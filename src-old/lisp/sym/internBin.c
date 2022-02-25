#include <lisp.h>

any internBin(Context *CONTEXT_PTR, any sym, any tree[2])
{
    any nm, x, y, z;
    word n;

    dump("internBin1");
    x = tree[1];

    if (isNil(x))
    {
        tree[1] = consIntern(CONTEXT_PTR, sym, Nil);
        return tree[1];
    }

    for (;;)
    {

        y = car(sym);
        z = car(car(x));
        while ((n = (word)(car(y)) - (word)car(z)) == 0)
        {
            if (GetType(y) != BIN) return sym;
            y=cdr(y);
            z=cdr(z);
        }

        if (isNil(cdr(x)))
        {
            dump("internBin1");
            if (n < 0)
            {
                dump("internBin2");
                any xx = consIntern(CONTEXT_PTR, sym, Nil);
                dump("internBin3");
                setCARType(xx, PTR_CELL);
                dump("internBin4");
                xx = consIntern(CONTEXT_PTR, xx, Nil);
                dump("internBin5");
                setCARType(xx, PTR_CELL);
                dump("internBin6");
                cdr(x) = xx;
                setCARType(x, PTR_CELL);
                dump("internBin7");
                return sym;
            }
            else
            {
                dump("internBin8");
                any xx = consIntern(CONTEXT_PTR, sym, Nil);
                dump("internBin9");
                setCARType(xx, PTR_CELL);
                dump("internBina");
                xx = consIntern(CONTEXT_PTR, Nil, xx);
                dump("internBinb");
                setCARType(xx, PTR_CELL);
                dump("internBinc");
                cdr(x) = xx;
                setCARType(x, PTR_CELL);
                dump("internBind");
                return sym;
            }

        }
        if (n < 0)
        {
            if (!isNil(cadr(x)))
            {
                x = cadr(x);
            }
            else
            {
                cadr(x) = consIntern(CONTEXT_PTR, sym, Nil);
                setCARType(car(x), PTR_CELL);
                return sym;
            }
        }
        else
        {
            if (!isNil(cddr(x)))
            {
                x = cddr(x);
            }
            else
            {
                cddr(x) = consIntern(CONTEXT_PTR, sym, Nil);
                setCARType(cdr(x), PTR_CELL);
                return sym;
            }
        }
    }
}
