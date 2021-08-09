#!/usr/bin/python3
import struct
import sys


def hash(inp):
    padsize = len(inp) % 4
    if padsize != 0:
        inp += 'X' * (4 - padsize)
    inp = inp.encode('latin1')
    res = "\x00" * 4
    res = bytearray(res, 'latin1')
    for i in range(0, len(inp), 4):
        res[0] ^= inp[i]
        res[1] ^= inp[i+1]
        res[2] ^= inp[i+2]
        res[3] ^= inp[i+3]
    return(struct.unpack("<i", res)[0])


print(hash(sys.argv[1]))
