## What is this?

This is an adaptation of [miniPicoLisp](https://picolisp.com/wiki/?embedded), a LISP interpreter. The primary goal of this adaptation is to be extremely readable and make vanilla C as the only dependency. It is not that PicoLisp implementation is not readable. It is my opinion however that some of the performance optimizations used in the PicoLisp implementation make it harder to understand the source. Plus, PicoLisp uses some _gcc only_ features. Please note that the readability goal is not achieved yet :)

## Why another language?

This is really not "another language" - it's essentially PicoLisp. 

## What is great about PicoLisp?

PicoLisp is the closest to perfection. Please remember, "Perfection is achieved, not when there is nothing more to add, but when there is nothing left to take away". A somewhat unique quality of PicoLisp is that it is ["finished software"](http://www.beneroth.ch/pil/picolisp-is-finished.html). Has been so for over 3 decades! This may be surprising to you, that a programming language could be "finished" so long ago; even though, we have "new features" being added to the prevailing popular programming languages all the time. 

PicoLisp is [powerful](https://picolisp.com/wiki/?pros-and-cons) and as [Paul Graham](https://twitter.com/paulg) stated in this wonderful essay ["Beating the averages"](http://www.paulgraham.com/avg.html) - programming languages vary in power.

### Comparing programming languages

Before



## If PicoLisp is so wonderful, why do you need an adaptation?

PicoLisp implementation is tied to POSIX/Linux and gcc more tightly that I'd like. Just like, "yes it works on Windows 98 and on Windows NT" is not reasonably acceptable when it comes to portablity; "WORKS" on POSIX is not sufficient in my opinion :) My goal is to have nothing more than vanilla C as a dependency - I say vanilla C to imply that C with very straight forward assembly.

## 


One of the things I like about PicoLisp is that it embraces the idea that a programmer could own and understand the programming lanugage. At its core, its really a virtual machine architecture - a very simple one at it. A singly linked list is almost all of it :) This deviates from how I'd viewed programming languages thus far.


After exploring a number of languages, I've arrived at the conclusion that PicoLisp is the best programming lauguage. While there is merit to the idea that there cannot really be "the best" computer programming language - I believe that it is posslbe to come up with a list of desirable qualities of a computer programming language and come up with "objective" ways to determine if various computer programming languages have the quality or not. I'll lay out the qualities and demostrate how PicoLisp is indeed the best programming language.




Let me address the "interpreter only" aspect first since this could be turn-off for some folks. It was for me. I had rejected PicoLisp in the past for just this reason. There are some interesting points about why the need for a compiler is unnecessary are mentioned [here](http://software-lab.de/radical.pdf) under section 2.1 - "Myth 1: Lisp needs a compiler".

It is truly simple - the VM of the interpreter is essentially a singly linked list. Which means that you can pretty much understand the complete implementation really quickly and would never have to depend on anyone (or any corp) for any feature.

### Building

```bash
git clone https://github.com/lispware/minilisp.git
cd minilisp
git submodule init
git submodule update
```

#### Windows

Assuming that you have run vcvars64.bat or vcvars32.bat to ensure that Micrsoft C compiler is set in the PATH.
```bash
nmake -f makefile
```

#### Linux and MacOS / OS X

Assuming that you have gcc
```bash
make
```

### Acknowledgements

1. PicoLisp community
2. [Will Portnoy](https://github.com/willportnoy)
