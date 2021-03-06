#!/bin/bash

if [ "all" == "$1" ]
then
echo gcc gen.c -o gen
gcc gen.c -o gen
echo ./gen mem.s
./gen mem.s
fi

SOURCES=$(cat sources.unix.list)

echo gcc -g3 mem64.c $SOURCES -o lisp64 -lpthread
gcc -g3 mem64.c $SOURCES -o lisp64 -lpthread
