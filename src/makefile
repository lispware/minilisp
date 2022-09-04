PARALLEL=-j6
OPT=-g3
MATH_OPT=-o3

SOURCES=main.c lisp.c unix.c gmp.c
EXE=lisp

64: $(SOURCES)
	$(CC) $(OPT) -DB64 -I . $(SOURCES) -lpthread -o $(EXE)

32: $(SOURCES)
	$(CC) -m32 $(OPT) -I . $(SOURCES)  -lpthread -o $(EXE)

leak64: $(SOURCES)
	$(CC) -fsanitize=address $(OPT) -I . $(SOURCES)  -lpthread -o $(EXE)

leak32: $(SOURCES)
	$(CC) -fsanitize=address -m32 $(OPT) -I . $(SOURCES) -lpthread -o $(EXE)
