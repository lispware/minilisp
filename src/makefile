include makefile.inc

# use make BITS=-m32 to build 32 bit lisp


ALL_SOURCES=$(SOURCES) \
			lisp/builtin/net/unix/putStdoutNet.c \
			lisp/builtin/net/unix/pltSocket.c \
			lisp/builtin/net/unix/getStdinNet.c \
			lisp/builtin/net/unix/pltListen.c \
			lisp/builtin/net/unix/pltBind.c \
			lisp/builtin/net/unix/pltClose.c \
			lisp/builtin/net/unix/popIOFilesNet.c \
			lisp/builtin/net/unix/pushIOFilesNet.c \
			lisp/builtin/net/unix/pltConnect.c \
			lisp/builtin/thread/unix/thread.c \
			lisp/builtin/thread/unix/sleep.c

OBJECTS=$(ALL_SOURCES:.c=.obj) libtommath/libtommath.a

all: $(OBJECTS)
	$(CC) $(OBJECTS) -lpthread libtommath/libtommath.a -o lisp.exe


single: lisp.c unix.c libtommath/libtommath.a
	$(CC) -I libtommath  lisp.c unix.c libtommath/libtommath.a -lpthread -o lisp.exe

libtommath/libtommath.a:
	cd libtommath && make CC="$(CC)"

%.obj:%.c
	$(CC) -I lisp -I libtommath -g3 -c $< -o $@

clean:
	find lisp -name "*.obj" -exec rm -f {} \;
	rm -f lisp.exe

cleanall:
	rm -f $(OBJECTS)
	rm -f lisp.exe
	cd libtommath && make clean
