CALL "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /x64 /release

set LIBRARY_INC=C:\conda\Library\include
set LIBRARY_LIB=C:\conda\Library\lib
set Scripts=C:\conda\Scripts

cl src\sweep2png.cpp src\png.cpp /Iinclude /I%LIBRARY_INC% %LIBRARY_LIB%\libpng.lib %LIBRARY_LIB%\zlib.lib

if not exist "%SCRIPTS%" mkdir %SCRIPTS%
cp sweep2png.exe %SCRIPTS%\sweep2png.exe
