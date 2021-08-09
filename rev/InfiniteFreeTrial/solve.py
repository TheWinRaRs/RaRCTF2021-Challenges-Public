#!/usr/bin/python3

def xor(a, b):
    out = []
    for x, y in zip(a, b):
        out.append(x^y)
    return out

def display(bs):
    for b in bs:
        print(chr(b), end='')
    print("")

xorcheck = [9,22,23,15,23,86,22,68,58,24,83,111,20,3,42,6,111,49,28,71,42,6,45,95,81,27,0,70,74,0,4,85,102,80,1,76]
flag = [ord(i) for i in 'rarctf']
for i in range(0, len(xorcheck), 6):
    flag.extend(xor(xorcheck[i:i+6], flag[-6:]))
display(flag)
