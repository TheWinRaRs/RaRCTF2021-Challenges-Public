from hashlib import md5
from binascii import hexlify

pw = b"bnnyr4bbits".ljust(16, b'\x00')
salt = b"https://i.imgur.com/gub160B.png".ljust(32, b'\x00')

hash1 = md5(pw).digest()

hash2 = md5(salt).digest()

print(md5(hash1 + hash2).hexdigest())
