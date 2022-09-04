cl /Zi -I WindowsSDL2/SDL2-2.0.20/include -I ../../src WindowsSDL2\SDL2-2.0.20\lib\x86\SDL2.lib ..\..\src\gmp.c ..\..\src\lisp.c ..\..\src\windows.c ws2_32.lib advapi32.lib  sdlmain.c /Felisp.exe
