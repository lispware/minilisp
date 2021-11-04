#!/bin/bash

SOURCES=$(cat sources.unix.list)

echo gcc -g3 $SOURCES -fsanitize=address -o lisp64 -lpthread
gcc -g3 $SOURCES  -o lisp64 -lpthread
