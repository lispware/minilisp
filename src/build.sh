#!/bin/bash

if [ "all" == "$1" ]
then
echo gcc gen.c -o gen
gcc gen.c -o gen
echo ./gen mem.s
./gen mem.s
fi

echo gcc lisp.c -o lisp
gcc lisp.c -o lisp
