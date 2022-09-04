REPO=../..
SOURCES=sdlmain.c $(REPO)/src/lisp.c $(REPO)/src/gmp.c $(REPO)/src/unix.c

64: $(SOURCES)
	cd ../../src && make
	cd -
	gcc -g3 -DB64 -D_OS_IS_LINUX_ `pkg-config --cflags sdl2` `pkg-config --cflags SDL2_ttf` `pkg-config --cflags SDL2_image` -I$(REPO)/src $(SOURCES) -o video `pkg-config --libs sdl2` `pkg-config --libs SDL2_ttf` `pkg-config --libs SDL2_image` -ldl -lpthread -o lisp

32: $(SOURCES)
	cd ../../src && make
	cd -
	gcc -m32 -g3 -D_OS_IS_LINUX_ `pkg-config --cflags sdl2` `pkg-config --cflags SDL2_ttf` `pkg-config --cflags SDL2_image` -I$(REPO)/src $(SOURCES) -o video `pkg-config --libs sdl2` `pkg-config --libs SDL2_ttf` `pkg-config --libs SDL2_image` -ldl -lpthread -o lisp

