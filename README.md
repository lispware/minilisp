# What is this?

This is an adaptation of [miniPicoLisp](https://picolisp.com/wiki/?embedded). The main difference from miniPicoLisp is that the core data structure CELL contains three parts instead of two. The third part is introduced to eliminate the need for pointer tagging. 

MiniLisp, like PicoLisp, is an *interpreter only* implementation. *Singly linked list is the only built in data structure* that this language provides. Just these may be sufficient for you to disregard this as a viable programming language. A few years ago, I had myself rejected PicoLisp just for those reasons. However, I have a very different opinon after I took a second look at PicoLisp after having more experience with Lisp/Clojure and having implemented a couple of Lisp compilers - [s2c](https://github.com/ckkashyap/s2c) for example.

## Building

```bash
git clone https://github.com/lispware/minilisp.git
cd minilisp
git submodule init
git submodule update
```

### Windows

Assuming that you have run vcvars64.bat or vcvars32.bat to ensure that Micrsoft C compiler is set in the PATH.
```bash
nmake -f makefile
```

### Linux and MacOS / OS X

Assuming that you have gcc
```bash
make
```

## Acknowledgements

1. PicoLisp community
2. [Will Portnoy](https://github.com/willportnoy)
