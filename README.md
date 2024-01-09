## What is this?

This is an adaptation of [miniPicoLisp](https://picolisp.com/wiki/?embedded), a LISP interpreter. The primary goal of this adaptation is to be extremely readable and make vanilla C as the only dependency. It is not that PicoLisp implementation is not readable. It is my opinion however that some of the space optimizations used in the PicoLisp implementation make it harder to understand the source. Plus, PicoLisp uses some _gcc only_ features. Please note that the readability goal is not achieved yet :)

## Why another programming language?

This is really not "another language" - it's essentially PicoLisp. 

## What is so great about PicoLisp?

PicoLisp is the closest to perfection. Perfection as in "Perfection is achieved, not when there is nothing more to add, but when there is nothing left to take away". A somewhat unique quality of PicoLisp is that it is ["finished software"](http://www.beneroth.ch/pil/picolisp-is-finished.html). Has been so for over 3 decades! This may be surprising to you, that a programming language could be "finished" so long ago; even though, we have "new features" being added to the prevailing popular programming languages all the time. 

PicoLisp is [powerful](https://picolisp.com/wiki/?pros-and-cons). I think that [Paul Graham](https://twitter.com/paulg)'s  wonderful essay ["Beating the averages"](http://www.paulgraham.com/avg.html) does a really good job in explaining the notion of the power of a programming language. The essay does not talk about PicoLisp specifically but this is a direct quote from the essay - "Lisp is so great not because of some magic quality visible only to devotees, but because it is simply the most powerful language available.".

## If PicoLisp is so wonderful, why do you need an adaptation?

PicoLisp implementation is tied to POSIX/Linux and gcc more tightly that I'd like. Just like, "yes it works on Windows 98 and on Windows NT" is not reasonably acceptable when it comes to portablity; "WORKS" on POSIX is not sufficient in my opinion :) My goal is to have nothing more than vanilla C as a dependency - I say vanilla C to imply that C with very straight forward assembly. PicoLisp also uses a number of space optimization techniques that I feel make it harder to understand the code and dependent on aligned allocations.

## Building

Build depends on libuv and libSDL2. Please take a look at the Dockerfile to see how they may be installed.

```bash
git clone https://github.com/lispware/minilisp.git
cd minilisp/src
make
```

#### Windows

Build depends on libuv and libSDL2. Prebuilt binaries for those are automatically fetched from https://github.com/ckkashyap/WindowsBinaries

Assuming that you have run vcvars64.bat or vcvars32.bat to ensure that Micrsoft C compiler is set in the PATH.
```bash
nmake -f nmakefile PLAT=[x64|x86]
```

#### GNULinux and MacOS / OS X

Assuming that you have gcc
```bash
make
```

### Acknowledgements

1. PicoLisp community
2. [Will Portnoy](https://github.com/willportnoy)
