@echo off
REM /F2147483648
set cmp=/Zi /FC
set inc=-I..\inc
set lnk=/SUBSYSTEM:CONSOLE ..\lib\*.lib opengl32.lib

IF NOT EXIST bin\ mkdir bin\
pushd bin\
cl %cmp% ..\hex.cpp %inc% /link %lnk%
popd