CALL "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" amd64
::dir %RECIPE_DIR%
xcopy %RECIPE_DIR%\stdint.h include\ /y

cl /MD src\sweep2png.c src\png.c /Iinclude /I%LIBRARY_INC% /link %LIBRARY_LIB%\libpng.lib %LIBRARY_LIB%\zlib.lib

if not exist "%SCRIPTS%" mkdir %SCRIPTS%
xcopy sweep2png.exe %SCRIPTS%\
