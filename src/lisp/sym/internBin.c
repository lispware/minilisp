#include <lisp.h>

any internBin(Context *CONTEXT_PTR, any sym, any tree[2])
{
    any nm, x, y, z;
    word n;

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
            cdr(x) = n < 0 ? consIntern(CONTEXT_PTR, consIntern(CONTEXT_PTR, sym, Nil), Nil) : consIntern(CONTEXT_PTR, Nil, consIntern(CONTEXT_PTR, sym, Nil));
            setCARType(x, PTR_CELL);
            dump("internBin2");
            return sym;
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
