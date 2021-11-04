#include <lisp.h>

any internBin(Context *CONTEXT_PTR, any sym, any tree[2])
{
    any nm, x, y, z;
    word n;

    x = tree[1];

    if (x == Nil)
    {
        tree[1] = consIntern(CONTEXT_PTR, sym, Nil);
        return tree[1];
    }

    for (;;)
    {

        y = sym->car;
        z = x->car->car;
        while ((n = (word)(car(y)) - (word)car(z)) == 0)
        {
            if (getCARType(y) != BIN) return sym;
            y=y->cdr;
            z=z->cdr;
        }

        if (Nil == cdr(x))
        {
            cdr(x) = n < 0 ? consIntern(CONTEXT_PTR, consIntern(CONTEXT_PTR, sym, Nil), Nil) : consIntern(CONTEXT_PTR, Nil, consIntern(CONTEXT_PTR, sym, Nil));
            return sym;
        }
        if (n < 0)
        {
            if (Nil != cadr(x))
            {
                x = cadr(x);
            }
            else
            {
                cadr(x) = consIntern(CONTEXT_PTR, sym, Nil);
                return sym;
            }
        }
        else
        {
            if (Nil != cddr(x))
            {
                x = cddr(x);
            }
            else
            {
                cddr(x) = consIntern(CONTEXT_PTR, sym, Nil);
                return sym;
            }
        }
    }
}
