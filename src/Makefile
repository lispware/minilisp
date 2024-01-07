# 12dec16abu
# (c) Software Lab. Alexander Burger

.SILENT:

bin = ../bin
picoFiles = main.c gc.c apply.c flow.c sym.c subr.c math.c io.c bufferAllocation.c lisp_sdl.c

CC = gcc $(PLAT)

picolisp: DEPS/SDL2-2.28.5 DEPS/libuv-v1.47.0 $(bin)/picolisp

.c.o:
	echo $*.c:
	$(CC) -c -g3 -pipe \
        -I DEPS/SDL2-2.28.5/INSTALL/include/SDL2 \
        -I DEPS/libuv-v1.47.0/INSTALL/include \
	-falign-functions -fomit-frame-pointer -fno-strict-aliasing \
	-W -Wimplicit -Wreturn-type -Wunused -Wformat \
	-Wuninitialized -Wstrict-prototypes \
	-D_GNU_SOURCE  $*.c

$(bin)/picolisp: $(picoFiles:.c=.o)
	mkdir -p $(bin)
	echo "  " link picolisp:
	$(CC) -o $(bin)/picolisp -L DEPS/libuv-v1.47.0/INSTALL/lib  -L DEPS/SDL2-2.28.5/INSTALL/lib $(picoFiles:.c=.o) -lc -lm -lSDL2 -luv
	strip $(bin)/picolisp

$(picoFiles:.c=.o): pico.h sym.d rom.d ram.d

sym.d rom.d ram.d: gen3m init.s lib.s pilog.s
	./gen3m 0 init.s lib.s pilog.s

gen3m: gen3m.c
	$(CC) -o gen3m gen3m.c

DEPS/SDL2-2.28.5: DEPS/SDL2-2.28.5.tar.gz
	mkdir -p DEPS/SDL2
	cd DEPS && tar -xzvf SDL2-2.28.5.tar.gz
	cd DEPS/SDL2-2.28.5 && ./configure --prefix=`pwd`/INSTALL && make all install 

DEPS/SDL2-2.28.5.tar.gz:
	mkdir -p DEPS
	cd DEPS && wget https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-2.28.5.tar.gz

DEPS/libuv-v1.47.0: DEPS/libuv-v1.47.0.tar.gz
	cd DEPS && tar -xzvf libuv-v1.47.0.tar.gz
	cd DEPS/libuv-v1.47.0 && ./autogen.sh && ./configure --prefix=`pwd`/INSTALL && make all install 

DEPS/libuv-v1.47.0.tar.gz:
	mkdir -p DEPS
	cd DEPS && wget https://dist.libuv.org/dist/v1.47.0/libuv-v1.47.0.tar.gz


# Clean up
clean:
	rm -f gen3m *.d *.o

# vi:noet:ts=4:sw=4
