CALL "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" amd64
xcopy conda.recipe\stdint.h include\ /y

REM edit below to find libpng and zlib
cl /MD src\sweep2png.c src\png.c /Iinclude /IC:\conda\Library\include /link C:\conda\Library\lib\libpng.lib C:\conda\Library\lib\zlib.lib

REM run a quick test
sweep2png.exe conda.recipe\Si_5grains_mesh.log conda.recipe/map/Si_70 0.7 cubic
