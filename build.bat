@echo off
set project=C:\Projects\Array30Ex

IF /i NOT "%1"=="CLEAN" GOTO COMPILE

cd %project%\Keyhook
mingw32-make.exe -f Makefile.gcc clean
cd %project%
mingw32-make.exe -f Makefile.gcc clean
del %project%\keyhook.dll
GOTO END

:COMPILE
cd %project%\Keyhook
mingw32-make.exe -f Makefile.gcc
COPY %project%\keyhook\keyhook.dll %project%\keyhook.dll
cd %project%
mingw32-make.exe -f Makefile.gcc

:END
