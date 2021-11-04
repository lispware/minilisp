@echo off

for /F "delims=" %%x in (sources.windows.list) do (set "%%x" )
cl /Zi %SOURCES% ws2_32.lib /Felisp64.exe
