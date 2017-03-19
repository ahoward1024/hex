@echo off

REM /F2147483648
set cmp=/Zi /FC
set inc=-I..\inc
set lnk=/SUBSYSTEM:CONSOLE ..\lib\*.lib ..\hex.res

IF NOT EXIST bin\ mkdir bin\
pushd bin\
cl %cmp% ..\hex.cpp %inc% /link %lnk%
popd
