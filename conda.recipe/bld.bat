gcc -static -g -O2 -Wall src/png.c src/sweep2png.c -Iinclude -I%LIBRARY_INC% -L%LIBRARY_LIB% -lpng -lz -o sweep2png

if not exist %SCRIPTS% mkdir -p %SCRIPTS%
xcopy sweep2png.exe %SCRIPTS%
