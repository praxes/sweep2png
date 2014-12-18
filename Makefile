all:
	gcc -g -O2 -Wall src/png.c src/sweep2png.c -Iinclude $(CFLAGS) -lpng -o sweep2png

install:
	mkdir -p $(PREFIX)/bin
	cp sweep2png $(PREFIX)/bin/sweep2png
