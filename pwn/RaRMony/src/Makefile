CC=gcc
.PHONY = all clean

all: clean logo.h harmony

harmony:
	${CC} harmony.c main.c -no-pie -o harmony

logo.txt:
	jp2a --width=70 --chars=' .oOM' WR.jpg | sed 'y/.oOM/░▒▓█/' > logo.txt

logo.h: logo.txt
	xxd -i $< > $@

clean:
	rm -f harmony logo.h logo.txt logo.h
