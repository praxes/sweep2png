CALL "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" amd64
xcopy conda.recipe\stdint.h include\ /y

REM edit below to find libpng
set LIBRARY_LIB=C:\conda\Library\lib
set LIBRARY_INC=C:\conda\Library\include

cl /MD src\sweep2png.c src\png.c /Iinclude /I%LIBRARY_INC% /link %LIBRARY_LIB%\libpng.lib
mt.exe -manifest sweep2png.exe.manifest -outputresource:sweep2png.exe;1

del sweep2png.exe.manifest

REM run a quick test
sweep2png.exe conda.recipe\Si_5grains_mesh.log conda.recipe/map/Si_70 0.7 cubic
