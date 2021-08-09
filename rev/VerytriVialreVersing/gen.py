flag = "rarctf{See,ThatWasn'tSoHard-1eb519ed}"
out = []

key = [0x13, 0x37]
for c in flag:
    c = ord(c)
    out.append((c ^ key[0]) + key[1])
    key = key[::-1]

# print(', '.join([chr(c) for c in out]))
print(', '.join([hex(c) for c in out]))
