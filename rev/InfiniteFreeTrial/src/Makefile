.PHONY: clean
.DEFAULT: ift

ift: ift.c gtk.c flag.h
	gcc gtk.c ift.c -o ift `pkg-config --cflags --libs gtk+-3.0`

flag.h: generate.py
	python3 generate.py > flag.h

clean:
	rm -f flag.h ift
