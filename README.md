# What is this?

This is an adaptation of [miniPicoLisp](https://picolisp.com/wiki/?embedded), a lisp interpreter. The primary goal of this adaptation is to be extremely readable and make vanilla C as the only dependency. It is not that PicoLisp implementation is not readable. It is my opinion however that some of the performance optimizations used in PicoLisp implementation make it harder to understand. Plus, PicoLisp uses some _gcc only_ features. Please note that the readability goal is not achieved yet :)

Let me address the "interpreter only" aspect first since this could be turn-off for some folks. It was for me. I had rejected PicoLisp in the past for just this reason. There are some interesting points about why the need for a compiler is unnecessary are mentioned [here](http://software-lab.de/radical.pdf) under section 2.1 - "Myth 1: Lisp needs a compiler".

It is truly simple - the VM of the interpreter is essentially a singly linked list. Which means that you can pretty much understand the complete implementation really quickly and would never have to depend on anyone (or any corp) for any feature.

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
