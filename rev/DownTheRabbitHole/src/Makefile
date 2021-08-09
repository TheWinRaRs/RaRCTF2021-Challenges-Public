CC = gcc
ASM = nasm
CFLAGS = -no-pie -fPIC -fPIE -fno-asynchronous-unwind-tables
frames = 0 1 2 3 4 5 6
nframes := $(shell echo $(frames) | wc -w)
key1 = $(shell python3 key1.py)
key2 = 643e0acb819ee3ad4d93761553fd2104

fakefs = framesrc/filesystem
.PHONY: all test clean filesystem

all: clean flag1.h headers xor loader


frames: filesystem
	$(info Compiling frames)
	@mkdir frames
	@for f in $(frames); do \
		${CC} framesrc/frame$$f.c -DKEY1=\"$(key1)\" -ffunction-sections -o frames/frame$$f.bin -fno-stack-protector -nostdlib -fno-asynchronous-unwind-tables -c -fPIC -fPIE ; \
		ld -T frame.ld -o frames/frame$$f.exe frames/frame$$f.bin; \
	done

headers: frames
	$(info Generating frame headers)
	@mkdir headers
	@for f in $(frames); do \
		./frameheaders.sh $$f ; \
	done

transitions: xor
	$(info Generating transitions)
	@echo "static unsigned char transitions[$(nframes)][$(nframes)][LENGTH] = {\\" >> headers/transitions.h;
	@for f1 in ${frames}; \
		do echo '{' >> headers/transitions.h; \
		for f2 in ${frames}; do \
			echo -n "TRANS_$$f1 _$$f2 , " | tr -d ' ' >> headers/transitions.h; \
		done; \
		echo '},'  >> headers/transitions.h; \
	done;
	@echo '};' >> headers/transitions.h;

loader: headers transitions
	$(info Compiling final binary)
	@${CC} loader.c xor.c ${CFLAGS} -DNFRAMES=$(nframes) -o loader
	@strip loader

xor: flag1.h headers frames
	@${CC} loader.c xor.c -DGENXOR -DNFRAMES=$(nframes) ${CFLAGS} -o xor
	@./xor >> headers/transitions.h

flag1.h: flag1.txt
	$(info Encrypting flag)
	@echo 'static const char flagenc[] = {' > flag1.h
	@openssl enc -in flag1.txt -K $(key1) -iv 0 -e -aes-128-cbc -nopad -nosalt | xxd -i >> flag1.h
	@echo '};' >> flag1.h

# $(echo $$file | rev | cut -d '/' -f1 | rev | sed -e 's/[[:punct:]]/_/g' | tr '[:lower:]' '[:upper:]') 

filesystem:
	$(info Generating encrypted files)
	@ls $(fakefs)/* | grep -v enc | while read f; do \
		openssl enc -in $$f -K $(key2) -iv 0 -e -aes-128-cbc -nopad -nosalt 2>/dev/null > $$f.enc ; done
	@ls $(fakefs)/* | grep enc | while read f; do \
		echo -n "#define $$(echo $$f | rev | cut -d '/' -f1 | rev | sed -e 's/[[:punct:]]/_/g' | tr '[:lower:]' '[:upper:]')_HASH " >> framesrc/filesystem.h ; \
		./fs-hash.py $$(echo $$f | sed 's/\.enc//g' | rev | cut -d '/' -f1 | rev) >> framesrc/filesystem.h ; \
		echo -n "static const "  >> framesrc/filesystem.h ; \
		xxd -i  $$f >> framesrc/filesystem.h; done


clean:
	$(info Cleaning up)
	rm -rf frames headers
	rm -f loader xor flag1.h $(fakefs)/*.enc framesrc/filesystem.h
