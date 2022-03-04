PARALLEL=-j6
OPT=-g3
MATH_OPT=-o3

SOURCES=main.c lisp.c unix.c 

64: $(SOURCES) libtommath64.a
	$(CC) $(OPT) -I libtommath $(SOURCES) libtommath64.a -lpthread -o lisp.exe

32: $(SOURCES) libtommath32.a
	$(CC) -m32 $(OPT) -I libtommath $(SOURCES) libtommath32.a -lpthread -o lisp.exe

leak64: $(SOURCES) libtommath_sanitize64.a
	$(CC) -fsanitize=address $(OPT) -I libtommath $(SOURCES) libtommath_sanitize64.a -lpthread -o lisp.exe

leak32: $(SOURCES) libtommath_sanitize32.a
	$(CC) -fsanitize=address -m32 $(OPT) -I libtommath $(SOURCES) libtommath_sanitize32.a -lpthread -o lisp.exe

libtommath64.a:
	cd libtommath && make -j16 clean
	cd libtommath && make -j16 CC="$(CC) $(MATH_OPT)"
	cp libtommath/libtommath.a libtommath64.a

libtommath32.a:
	cd libtommath && make -j16 clean
	cd libtommath && make -j16 CC="$(CC) $(MATH_OPT) -m32"
	cp libtommath/libtommath.a libtommath32.a

libtommath_sanitize64.a:
	cd libtommath && make -j16 clean
	cd libtommath && make -j16 CC="$(CC) $(MATH_OPT) -fsanitize=address"
	cp libtommath/libtommath.a libtommath_sanitize64.a

libtommath_sanitize32.a:
	cd libtommath && make -j16 clean
	cd libtommath && make -j16 CC="$(CC) $(MATH_OPT) -m32 -fsanitize=address"
	cp libtommath/libtommath.a libtommath_sanitize32.a
