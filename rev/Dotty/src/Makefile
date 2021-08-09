.PHONY=all

all: Dotty.exe

Dotty.exe: Dotty.cs Check.cs
	mcs /out:Dotty.exe Dotty.cs Check.cs

Check.cs: generate.py
	python3 generate.py

clean:
	rm -f Check.cs Dotty.exe
