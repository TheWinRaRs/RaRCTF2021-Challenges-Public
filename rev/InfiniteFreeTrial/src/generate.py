import crc8
import random
flag = b"rarctf{welc0m3_t0_y0ur_new_tr14l_281099b9}"
check = "w1nR4rs"


assert(len(flag)/len(check) == float(len(flag)//len(check)))
chunk_size = len(flag)//len(check)

print(f"#define CHUNKSZ {chunk_size}")
print(f"#define NCHUNKS {len(check)}")

buf = [None for _ in range(256)]
for i in range(0, len(flag), chunk_size):
    h = crc8.crc8()
    h.update(flag[i:i+chunk_size])
    result = h.digest()[0]
    assert buf[result] is None
    buf[result] = ord(check[i//chunk_size])

all = {x for x in range(256)}
not_set = list(all-set(buf))
random.shuffle(not_set)
for i in range(len(buf)):
    if not buf[i]:
        buf[i] = not_set.pop()
print('const char crccheck[] = {', end='')
print(', '.join([str(x) for x in buf]), end='')
print('};')

def xor(c1, c2):
    longer = max([len(c1), len(c2)])
    c1.ljust(longer, b'\x00')
    c2.ljust(longer, b'\x00')
    o = b""
    for a, b in zip(c1, c2):
        o += chr(a^b).encode('latin1')
    return o

check2 = b""
for i in range(0, len(flag), chunk_size):
    check2 += xor(flag[i:i+chunk_size], flag[i+chunk_size:i+chunk_size+chunk_size])
print("const char xorcheck[] = {", end='');
print(','.join([str(x) for x in check2]), end='')
print('};')
