CALL "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /x64

REM uncomment below to build against the locally-built libpng:
cl src\sweep2png.c src\png.c /Iinclude /Ilpng1615 /link lpng1615\libpng.lib C:\conda\Library\lib\zlib.lib

cd conda.recipe
..\sweep2png Si_5grains_mesh.log map/Si_70 0.7 cubic
cd ..

REM uncomment below to build against anaconda's libpng:
cl src\sweep2png.c src\png.c /Iinclude /IC:\conda\Library\include /link C:\conda\Library\lib\libpng.lib C:\conda\Library\lib\zlib.lib

cd conda.recipe
..\sweep2png Si_5grains_mesh.log map/Si_70 0.7 cubic
cd ..
