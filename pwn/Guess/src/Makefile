.PHONY = all clean

all: guess

guess: guess.c
	gcc guess.c -g -o guess -fstack-protector -masm=intel

clean:
	rm -f guess