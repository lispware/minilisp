REPO=../../..
SOURCES=sdlmain.c $(REPO)/src/lisp.c $(REPO)/src/unix.c

64: $(SOURCES)
	gcc -D_OS_IS_LINUX_ -I$(REPO)/src/libtommath $(SOURCES) $(REPO)/src/libtommath64.a -o video -lSDL2 -ldl -lpthread

32: $(SOURCES)
	gcc -m32 -D_OS_IS_LINUX_ -I$(REPO)/src/libtommath $(SOURCES) $(REPO)/libtommath64.a -o video -lSDL2 -ldl -lpthread
