#!/bin/bash

if [ "all" == "$1" ]
then
echo gcc gen.c -o gen
gcc -m32 gen.c -o gen
echo ./gen mem.s
./gen mem.s
fi

SOURCES=$(cat sources.unix.list)

echo gcc -m32 -g3 mem32.c $SOURCES -o lisp32 -lpthread
gcc -m32 -g3 mem32.c $SOURCES -o lisp32 -lpthread
