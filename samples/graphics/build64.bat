rem REMEMBER to replace all long's in gmp.c and gmp.h to long long
cl -DB64 /Zi -I WindowsSDL2/SDL2-2.0.20/include -I ../../src ../../src/gmp.c WindowsSDL2\SDL2-2.0.20\lib\x64\SDL2.lib ..\..\src\lisp.c ..\..\src\windows.c ws2_32.lib advapi32.lib  sdlmain.c /Felisp.exe
