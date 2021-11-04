#!/bin/bash

SOURCES=$(cat sources.unix.list)

echo gcc -m32 -g3 $SOURCES -o lisp32 -lpthread
gcc -m32 -g3 $SOURCES -o lisp32 -lpthread
