@echo off
if "%1" == "all"  goto buildgen
goto buildlisp
:buildgen
cl /Zi gen.c
gen mem.s
:buildlisp

for /F "delims=" %%x in (sources.windows.list) do (set "%%x" )
cl /Zi mem32.c %SOURCES% ws2_32.lib /Felisp32.exe
