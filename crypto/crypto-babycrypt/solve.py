from pwn import *
from gmpy2 import iroot
from Crypto.Util.number import long_to_bytes

s = remote(sys.argv[1],int(sys.argv[2]))
data = s.recvall().decode().split("\n")
e,n = eval(data[0].split(":")[1])
hint = eval(data[1].split(":")[1])
ct = eval(data[2].split(":")[1])
r = hint**2 + (4 * n)
r = int(iroot(r,2)[0])
p = (r + hint)//2 # only the positive root
q = n // p
assert p * q == n
d = pow(e,-1,(p-1)*(q-1))
flag = long_to_bytes(pow(ct,d,n)).decode()
print(flag)
