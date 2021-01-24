#!/bin/bash

if [ "all" == "$1" ]
then
echo gcc gen.c -o gen
gcc -m32 gen.c -o gen
echo ./gen mem.s
./gen mem.s
fi

echo gcc -g3 lisp.c cell.c -o lisp
gcc -m32 -g3 lisp.c cell.c -o lisp
