@echo off
if "%1" == "all"  goto buildgen
goto buildlisp
:buildgen
cl /Zi gen.c
gen mem.s
:buildlisp
cl /Zi cImpls.c lisp.c cell.c mem32.c /o lisp32.exe
