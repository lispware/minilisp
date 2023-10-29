@echo off

cl /nologo gen3m.c /Fe:gen3m.exe

gen3m.exe 0 init.s lib.s pilog.s

set OPT=/Ox
cl /nologo %OPT% /DMICROSOFT_C /c main.c
cl /nologo %OPT% /DMICROSOFT_C /c gc.c
cl /nologo %OPT% /DMICROSOFT_C /c apply.c
cl /nologo %OPT% /DMICROSOFT_C /c flow.c
cl /nologo %OPT% /DMICROSOFT_C /c sym.c
cl /nologo %OPT% /DMICROSOFT_C /c subr.c
cl /nologo %OPT% /DMICROSOFT_C /c math.c
cl /nologo %OPT% /DMICROSOFT_C /c io.c

cl /nologo main.obj gc.obj apply.obj flow.obj sym.obj subr.obj math.obj io.obj /Fe:picolisp.exe
