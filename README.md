# MiniLisp

This is an adaptation of [miniPicoLisp](https://picolisp.com/wiki/?embedded). The main difference from miniPicoLisp is that the core data structure CELL contains three parts instead of two. The third part is introduced to eliminate the need for pointer tagging. 


## Building

lisp.c is the LISP interpreter and can be built using a regular C compiler as follows

### Windows

```bash
cl lisp.c
```

### Linux and MacOS / OS X

```bash
gcc lisp.c -o lisp
```
