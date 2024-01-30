# 12dec16abu
# (c) Software Lab. Alexander Burger

.SILENT:

bin = ../bin
picoFiles = main.c gc.c apply.c flow.c sym.c subr.c math.c io.c lisp_sdl_libuv.c lisp_sdl_libuv_stub.c

ifdef USE_LIBUV_AND_LIBSDL
CC=gcc -DUSE_LIBUV_AND_LIBSDL
LIBS=-lSDL2 -luv
else
CC = gcc
endif

picolisp: $(bin)/picolisp

.c.o:
	echo $*.c:
	$(CC) -c -O3 -pipe \
        -I /usr/local/include/SDL2 \
	-falign-functions -fomit-frame-pointer -fno-strict-aliasing \
	-W -Wimplicit -Wreturn-type -Wunused -Wformat \
	-Wuninitialized -Wstrict-prototypes \
	-D_GNU_SOURCE  $*.c

$(bin)/picolisp: $(picoFiles:.c=.o)
	mkdir -p $(bin)
	echo "  " link picolisp:
	$(CC) -o $(bin)/picolisp $(picoFiles:.c=.o) -lc -lm $(LIBS) 
	strip $(bin)/picolisp

$(picoFiles:.c=.o): pico.h sym.d rom.d ram.d

sym.d rom.d ram.d: gen3m init.s lib.s pilog.s
	./gen3m 0 init.s lib.s pilog.s

gen3m: gen3m.c
	$(CC) -o gen3m gen3m.c

# Clean up
clean:
	rm -f gen3m *.d *.o

# vi:noet:ts=4:sw=4
