# 12dec16abu
# (c) Software Lab. Alexander Burger

.SILENT:

bin = ../bin
picoFiles = main.c gc.c apply.c flow.c sym.c subr.c math.c io.c bufferAllocation.c lisp_sdl.c

CC = gcc $(PLAT)

picolisp: $(bin)/picolisp

.c.o:
	echo $*.c:
	$(CC) -c -O2 -pipe \
        -I /home/kashyap/s/SDL2-install/include \
        -I /home/kashyap/s/libuv-install/include \
	-falign-functions -fomit-frame-pointer -fno-strict-aliasing \
	-W -Wimplicit -Wreturn-type -Wunused -Wformat \
	-Wuninitialized -Wstrict-prototypes \
	-D_GNU_SOURCE  $*.c

$(bin)/picolisp: $(picoFiles:.c=.o)
	mkdir -p $(bin)
	echo "  " link picolisp:
	$(CC) -o $(bin)/picolisp -L /home/kashyap/s/libuv-install/lib  -L /home/kashyap/s/SDL2-install/lib $(picoFiles:.c=.o) -lc -lm -lSDL2 -luv
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