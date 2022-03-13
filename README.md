## What is this?

This is an adaptation of [miniPicoLisp](https://picolisp.com/wiki/?embedded), a LISP interpreter. The primary goal of this adaptation is to be extremely readable and make vanilla C as the only dependency. It is not that PicoLisp implementation is not readable. It is my opinion however that some of the space optimizations used in the PicoLisp implementation make it harder to understand the source. Plus, PicoLisp uses some _gcc only_ features. Please note that the readability goal is not achieved yet :)

## Why another programming language?

This is really not "another language" - it's essentially PicoLisp. 

## What is great about PicoLisp?

PicoLisp is the closest to perfection. Perfection as in "Perfection is achieved, not when there is nothing more to add, but when there is nothing left to take away". A somewhat unique quality of PicoLisp is that it is ["finished software"](http://www.beneroth.ch/pil/picolisp-is-finished.html). Has been so for over 3 decades! This may be surprising to you, that a programming language could be "finished" so long ago; even though, we have "new features" being added to the prevailing popular programming languages all the time. 

PicoLisp is [powerful](https://picolisp.com/wiki/?pros-and-cons). I think that [Paul Graham](https://twitter.com/paulg)'s  wonderful essay ["Beating the averages"](http://www.paulgraham.com/avg.html) does a really good job in explaining the notion of the power of a programming language. The essay does not talk about PicoLisp specifically but this is a direct quote from the essay - "Lisp is so great not because of some magic quality visible only to devotees, but because it is simply the most powerful language available.".

### Comparing programming languages

Some people discard the idea program

#### Non criteria

**Static vs Dynamic typing**
One argument that supports Static typing is that it helps in proving correctness. The argument against it is that one can only prove that the types line up - whether the program is correct from the programmer intention point of view is questionable. Second, it is argued that static types help with refactoring. Again, to the extent of types aligning. Remember that the following is correct Haskell code - as in it compiles :)
```haskell
sum = foldr (-) 0
```

One of [Larry Wall](http://www.wall.org/~larry/)'s many wonderfully hilarious one liners is - "A Perl program is correct if it gets the job done before your boss fires you." :)

The argument for dynamic type system, in my opinion is flawed - they talk about "compiler does not come in the way" etc. - as in, one is allowed to write "incorrect code" and are told about it only at run time. Anyway, my point is - if you have to shoot for correctness and speed (development and runtime) - shoot for a good design first. The rest will follow.

Design is a somewhat diluted term in the software industry so I'll go with Rich Hickey's [definition](https://www.youtube.com/watch?v=MCZ3YgeEUPg&t=184s) - which is, I paraphrase here - the art of breaking down a problem such that it can be composed back. Essentially, your implementation must contain tiny modules each of which do one thing. This, will help writing "evident code" that is "obviously correct; instead of, without obvious mistakes"[Quoting Simon Peyton Jones]. The type system of the language per se does not help or come in the way of design.

**IDE support**
I am just calling this out since I often here this as the reason for chosing a certain language. As a matter of fact, I think that IDEs are counter productive for the language design evolution. If language designers made it a requirement that the language should be useable in a simple text editor, then I believe that it would result in succinct constructs. The prevailing popular programming languages are almost impossible to write without the support of IDE support.

**Indentation level as block structure**
I can totally get it if you hate the idea. I myself did not like it one bit. It was only after I studied Haskell that I learnt to bear it and later love it.

So I feel - "Don't judge a language by it's choice for block structure"!

**Software Transaction memory, CSP style channels**
Clojure has demonstrated that these things can be built as a library when your language is powerful enough.


* b
* c

## If PicoLisp is so wonderful, why do you need an adaptation?

PicoLisp implementation is tied to POSIX/Linux and gcc more tightly that I'd like. Just like, "yes it works on Windows 98 and on Windows NT" is not reasonably acceptable when it comes to portablity; "WORKS" on POSIX is not sufficient in my opinion :) My goal is to have nothing more than vanilla C as a dependency - I say vanilla C to imply that C with very straight forward assembly.

PicoLisp also uses a number of space optimization techniques that I feel make it harder to understand the code and dependent on aligned allocations.






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
