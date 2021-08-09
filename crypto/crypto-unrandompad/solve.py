from pwn import *
from sympy.ntheory.modular import crt
from gmpy2 import iroot
from Crypto.Util.number import long_to_bytes

ns = []
cs = []
for _ in range(3):
  s = remote(sys.argv[1], int(sys.argv[2]))
  s.recvuntil("n: ")
  ns.append(int(s.recvline().decode()))
  s.sendlineafter("opt: ", "2")
  s.recvuntil("c: ")
  cs.append(int(s.recvline().decode()))
  s.close()

ptc = int(crt(ns, cs)[0])
print(long_to_bytes(int(iroot(ptc, 3)[0])).decode())
