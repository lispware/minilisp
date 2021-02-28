#!/bin/bash

if [ "all" == "$1" ]
then
echo gcc gen.c -o gen
gcc -m32 gen.c -o gen
echo ./gen mem.s
./gen mem.s
fi

echo gcc -m32 -g3 sym.c read.c gc.c flow.c lisp.c cell.c mem32.c math.c platform/unix/thread.c -o lisp32 -lpthread
gcc -m32 -g3 sym.c read.c gc.c flow.c lisp.c cell.c mem32.c math.c platform/unix/thread.c -o lisp32 -lpthread
