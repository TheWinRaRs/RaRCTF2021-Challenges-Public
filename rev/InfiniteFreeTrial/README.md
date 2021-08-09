# Infinite Free Trial
## Rev

We are asked to input a key in order to register the application. By reversing the binary, we can quickly see two functions: `do_crc_check` and `do_xor_check`.

The CRC function takes the input in chunks of 6, and performs a CRC8 calculation on them. The resulting byte is used as an index into an array of all bytes in a random order, `crccheck`. The resulting bytes extracted are checked to be `w1nR4rs`.

The XOR check takes the input in chunks of 6, and XOR's each block with the following block. The results are then compared to `xorcheck`.

As we know the format of the flag is `rarctf{`, we can actually reverse a valid key using only the XOR function.
```
check1 = block1 ^ block2 = 'rarctf' ^ block2
Therefore,
block2 = 'rarctf' ^ check1
check2 = block2 ^ block3 = ('rarctf' ^ check1) ^ block3
Therefore,
block3 = ('rarctf' ^ check1) ^ check2
```
and so on, until we have the flag.
`rarctf{welc0m3_t0_y0ur_new_tr14l_281099b9}`
