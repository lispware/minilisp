pushd .
cd ../../src/libtommath && nmake -f makefile.msvc
popd
cl /Zi -I WindowsSDL2/SDL2-2.0.20/include -I ../../src -I ../../src/libtommath ../../src/libtommath/tommath.lib WindowsSDL2\SDL2-2.0.20\lib\x64\SDL2.lib ..\..\src\lisp.c ..\..\src\windows.c ws2_32.lib advapi32.lib  sdlmain.c /Felisp.exe
