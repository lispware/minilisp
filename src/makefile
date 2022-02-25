all: lisp.c unix.c libtommath/libtommath.a
	$(CC) -g3 -I libtommath  lisp.c unix.c libtommath/libtommath.a -lpthread -o lisp.exe

libtommath/libtommath.a:
	cd libtommath && make CC="$(CC)"
