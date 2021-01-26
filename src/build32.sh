#!/bin/bash

if [ "all" == "$1" ]
then
echo gcc gen.c -o gen
gcc -m32 gen.c -o gen
echo ./gen mem.s
./gen mem.s
fi

echo gcc -m32 -g3 cImpls.c lisp.c cell.c mem32.c -o lisp32
gcc -m32 -g3 cImpls.c lisp.c cell.c mem32.c -o lisp32
