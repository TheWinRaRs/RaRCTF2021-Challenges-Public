frame=$1
objcopy -O binary --only-section .text ./frames/frame$1.exe ./frames/frame$1.dat
echo "#define FRAME$frame \"$(od -An -t x1  ./frames/frame$1.dat | sed 's/ /\\x/g' | tr -d \\n)\"" >> headers/frames.h
# echo "#define FRAMEENTRY$frame 0x$(nm frames/frame$frame.exe | grep entry | cut -d ' ' -f1) - 0xabcdef000" >> headers/frames.h
echo "#define FRAMEENTRY$frame 0x$(nm frames/frame$frame.bin | grep entry | cut -d ' ' -f1)" >> headers/frames.h
