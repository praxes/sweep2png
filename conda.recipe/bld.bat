CALL "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" amd64

xcopy %RECIPE_DIR%\stdint.h include\ /y

cl /MD src\sweep2png.c src\png.c /Iinclude /I%LIBRARY_INC% /link %LIBRARY_LIB%\libpng_static.lib %LIBRARY_LIB%\zlibstatic.lib
mt.exe -manifest sweep2png.exe.manifest -outputresource:sweep2png.exe;1

if not exist "%SCRIPTS%" mkdir %SCRIPTS%
xcopy sweep2png.exe %SCRIPTS%\
