@echo off
if "%1" == "all"  goto buildgen
goto buildlisp
:buildgen
cl /Zi gen.c
gen mem.s
:buildlisp
cl /Zi flow.c lisp.c cell.c mem64.c math.c /o lisp64.exe
