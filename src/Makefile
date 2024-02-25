# 12dec16abu
# (c) Software Lab. Alexander Burger

.SILENT:

bin = ../bin
picoFiles = main.c gc.c apply.c flow.c sym.c subr.c math.c io.c lisp_libsdl.c lisp_libsdl_stub.c lisp_libuv.c lisp_libuv_stub.c libbf.c cutils.c mathLIBBF.c

CC = gcc
LIBS=

ifdef DONT_USE_LIBSDL
CC+= -DDONT_USE_LIBSDL
else
LIBS+= -lSDL2
endif

ifdef DONT_USE_LIBUV
CC+= -DDONT_USE_LIBUV
else
LIBS+= -luv
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
	$(CC) -o $(bin)/picolisp $(picoFiles:.c=.o) -lc $(LIBS) 
	strip $(bin)/picolisp

$(picoFiles:.c=.o): pico.h sym.d rom.d ram.d

sym.d rom.d ram.d: gen3m init.s lib.s pilog.s libuv.s libsdl.s libbf.s
	./gen3m 0 init.s lib.s pilog.s libuv.s libsdl.s libbf.s

gen3m: gen3m.c
	$(CC) -o gen3m gen3m.c

# Clean up
clean:
	rm -f gen3m *.d *.o

# vi:noet:ts=4:sw=4
