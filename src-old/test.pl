#!/usr/bin/perl

`rm lisp64 lisp32`;


sub test
{
    my($build, $test, $baseline, $name)=@_;
    print "$name...$build...";
    `$build`;
    print "$test...";
    `$test`;
    my$diff = `diff $baseline out`;
    print "PASS $name\n" if $diff eq "";
    print "** FAIL ** $name\n" if $diff ne "";
    print "\n";
}

&test("./build64.sh all", "./lisp64 t.l > out", "baseline64.output", "lisp64");
&test("./build32.sh all", "./lisp32 t.l > out", "baseline32.output", "lisp32");
&test("gcc main.c", "./a.out test.l > out", "out64", "main.c 64 bit");
&test("gcc -m32 main.c", "./a.out test.l > out", "out32", "main.c 32 bit");

