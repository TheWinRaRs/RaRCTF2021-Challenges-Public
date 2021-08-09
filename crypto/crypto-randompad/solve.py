## Warning, this is bad code and extremely messy
from pwn import *
from Crypto.Util.number import long_to_bytes, bytes_to_long
from mt19937predictor import MT19937Predictor
from gmpy2 import iroot

s = remote(sys.argv[1],int(sys.argv[2]))
s.recvline()
e = 3
n = int(s.recvline().decode().split(": ")[1])

rng = MT19937Predictor()

def pad(m, n): # pkcs#1 v1.5
  ms = m.to_bytes((m.bit_length() + 7) // 8, "big")
  ns = n.to_bytes((n.bit_length() + 7) // 8, "big")
  if len(ms) > len(ns) - 11: return -1
  padlength = len(ns) - len(ms) - 3
  ps = long_to_bytes(random.getrandbits(padlength * 8)).rjust(padlength, b"\x00")
  return ps

print("params got")
pt = 1 << 1356
while len(pad(pt, n)) != 80:
  pt <<= 1                                                                                                                                                                                                                                 
                                                                                                                                                                                                                                           
for _ in range(125):   
  print(_)
  s.recvuntil("opt: ")                                                                                                                                                                                                                     
  s.sendline("1")                                                                                                                                                                                                                          
  s.sendline(str(pt))                                                                                                                                                                                                                      
  s.recvuntil("c: ")                                                                                                                                                                                                                       
  af = int(s.recvline().decode())                                                                                                                                                                                                          
  af = iroot(((af * pow(pow(pt,3),-1,n)) % n), 3)[0] & 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff0000
  af >>= 16
  while af != 0:
    rng.setrandbits(af & 0xffffffff, 32)
    af >>= 32
thonk = [rng.getrandbits(32) for _ in range(100)][::-1]
stuff = 0
for x in thonk:
  stuff <<= 32
  stuff += x


s.recvuntil("opt: ")
s.sendline("2")
s.recvuntil("c: ")
flag = int(s.recvline().decode())

s.recvuntil("opt: ") # Send another one to get 4 possibilities for flag length, then just brute them all :thumbsup:
s.sendline("1")
s.sendline(str(pt))
s.recvuntil("c: ")
af = int(s.recvline().decode())
af = iroot(((af * pow(pow(pt,3),-1,n)) % n), 3)[0] & 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff0000
print(hex(stuff))
print(f'rand = {hex(af)}')
print(f'ct  = {flag}')
print(f'n = {n}')
