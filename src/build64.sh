#!/bin/bash

if [ "all" == "$1" ]
then
echo gcc gen.c -o gen
gcc gen.c -o gen
echo ./gen mem.s
./gen mem.s
fi

echo gcc -g3 cImpls.c lisp.c cell.c mem64.c math.c -o lisp64
gcc -g3 cImpls.c lisp.c cell.c mem64.c math.c -o lisp64
