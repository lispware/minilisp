REPO=../../..
SOURCES=sdlmain.c $(REPO)/src/lisp.c $(REPO)/src/unix.c

64: $(SOURCES)
	gcc -g3 -D_OS_IS_LINUX_ -I$(REPO)/src -I$(REPO)/src/libtommath $(SOURCES) $(REPO)/src/libtommath64.a -o video -lSDL2  -lSDL2_ttf -lSDL2_image -ldl -lpthread

32: $(SOURCES)
	gcc -g3 -m32 -D_OS_IS_LINUX_ -I$(REPO)/src/libtommath $(SOURCES) $(REPO)/libtommath64.a -o video -lSDL2  -lSDL2_ttf -lSDL2_image -ldl -lpthread
