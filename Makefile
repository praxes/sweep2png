all:
	gcc -g -O2 -Wall src/png.cpp src/sweep2png.cpp -Iinclude $(CFLAGS) -lpng -o sweep2png

install:
	mkdir -p $(PREFIX)/bin
	cp sweep2png $(PREFIX)/bin/sweep2png
