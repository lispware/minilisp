@echo off
if "%1" == "all"  goto buildgen
goto buildlisp
:buildgen
cl /Zi gen.c
gen mem.s
:buildlisp
cl /Zi sym.c read.c gc.c flow.c lisp.c cell.c mem64.c math.c platform/windows/thread.c /Felisp64.exe
