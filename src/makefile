bin = ../bin
picoFiles = main.c gc.c apply.c flow.c sym.c subr.c math.c io.c libuv.c
objFiles = $(picoFiles:.c=.o)

$(bin)/picolisp:  sym.d rom.d ram.d $(objFiles)
	mkdir -p $(bin)
	$(CC) -o $(bin)/picolisp  -L $(UVDIST)/lib $(picoFiles:.c=.o) -lc -lm -luv
	strip $(bin)/picolisp

gen3m: gen3m.c
	$(CC) -o gen3m gen3m.c

sym.d rom.d ram.d functions.d: gen3m init.s lib.s pilog.s
	./gen3m 0 init.s lib.s pilog.s

.c.o: 
	echo $*.c:
	$(CC) -c -O2 -pipe \
	-falign-functions -fomit-frame-pointer -fno-strict-aliasing \
	-W -Wimplicit -Wreturn-type -Wunused -Wformat \
	-Wuninitialized -Wstrict-prototypes \
	-D_GNU_SOURCE  $*.c

libuv.o: sym.d rom.d ram.d functions.d libuv.c
	echo ABCDEFG
	$(CC) -c -O2 -pipe \
	-falign-functions -fomit-frame-pointer -fno-strict-aliasing \
	-W -Wimplicit -Wreturn-type -Wunused -Wformat \
	-Wuninitialized -Wstrict-prototypes \
	-I $(UVDIST)/include -L $(UVDIST)/lib \
	-D_GNU_SOURCE  libuv.c

clean:
	rm *.o *.d $(bin)/picolisp
