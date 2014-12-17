mv %RECIPE_DIR%\stdint.h %SRC_DIR%\include\
cl /o sweep2png.exe src\sweep2png.cpp src\png.cpp /Iinclude /I%LIBRARY_INC% /Ilpng1615 lpng1615\libpng.lib zlib\zlib.lib

mv sweep2png.exe %SCRIPTS%
