
# Down The Rabbit Hole



The main idea of this challenge is we have a small loader program, which loads a series of heavily obfuscated sets of independent shellcode into an RWX segment. The difficulty comes from the fact that rather than storing these frames in the binary, they are stored as a series of 'transitions'; every single frame has an XOR mask generated for every other frame; this allows transitioning between any frame at any point.
Every frame byte is also XOR'd with 0x90 to prevent strings from appearing in the final binary. None of the strings that would be visible would spoil the challenge, so only minor obfuscation is applied here.


### Compilation Process

First, each `frame<n>.c` file is compiled. To allow code that is both independent and small, basic syscalls are implemented in assembly, and stdlib functions are implemented and included using these syscall primitives. A linker script is then used to ensure important data is in the .text segment.

We then iterate over each compiled frame, and dump the .text segment out as a string of bytes into a header file. We also extract the address of the `entry` function: due to the inclusion of these library functions, the actual entrypoint appears last so its offset must be recorded into the headers.

Then, the binary is compiled with the 'GENXOR' preprocessor flag. When this is set, an array of frame shellcode strings are created in the binary. We then iterate over the number of frames in a nested for loop: essentially XORing two frames together, and outputting a `#define` statement containing the resulting mask as a string constant.

Finally, the binary is recompiled without this flag. We now create a 3D array of the transitions, map a large section in memory RWX and copies the first frame in.

When the `execute_frame(n)` function is called, the required mask is retrieved from the array with `transitions[current_frame][n]`, and applied to the mapped memory region byte by byte. Then we just cast the base address to a function pointer, and jump to it.



### IPC

To make dynamic analysis harder, the process forks, and uses unidirectional unix pipes to communicate. This is accomplished by mapping the executable space as PRIVATE (so each fork can have its own) but defining a sharedmem struct, which uses a SHARED mapping to hold shared data. For example, the first frame uses the pipe syscall to generate a pipe, and writes the two generated file descriptions to shm->pipe1[2]. The process then forks, with one frame reading input from the user and writing it to a pipe, and the other reading the input from the pipe and checking it.

### Flag Part 1
This part asks the user for a password. If correct they are given the flag; if incorrect they are told so.
The user input is sent ( along with a constant string) via a pipe to a hasher process. This md5 hashes the input and the constant string, concatenates the resulting hashes, then md5 hashes the result. This hash is sent down a pipe to a decryptor process, which first hexlifies the hash and checks if it matches the expected one. If it does, it is used as an AES key to decrypt the first flag part.

### Flag Part 2
This part has a pseudoshell, allowing listing and reading of files (embedded in the binary). `cat flag2.txt` results in a warning. If this is bypassed, the flag is printed.
The parent process reads the user input, and sends it down a pipe to the command processor process. This process constantly loops, reading from the cmd pipe and sending results to the result pipe. Commands such as `whoami` and `ls` are handled by this, but `cat` commands cause the filename to be sent down another pipe to the filesystem process.
Filenames are passed through a simple hashing function, and the result is used in a switch case statement to retrieve and decrypt the file. Before this though, the filename is checked to see if it begins with `flag2.txt`; if it does, a warning is sent back. The hashing function can be abused to cause a collision, which will not match `flag2.txt`. This results in the second flag part being printed.
