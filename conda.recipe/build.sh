gcc -g -O2 -Wall src/png.c src/sweep2png.c -Iinclude -I$PREFIX/include -L$PREFIX/lib -lpng -o sweep2png

mkdir -p $(PREFIX)/bin
cp sweep2png $(PREFIX)/bin/sweep2png
