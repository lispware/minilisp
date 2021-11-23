#include <lisp.h>

any popSym(Context *CONTEXT_PTR, int i, uword n, any q, cell *cp)
{
    if (q)
    {
        //val(q) = i <= (BITS-2)? box(n) : consName(CONTEXT_PTR, n, Zero);
        q->cdr = consName(CONTEXT_PTR, n, Nil);
        return Pop(*cp);
    }
    else
    {
        cell c1;
        any x = consSym(CONTEXT_PTR, NULL, 0);
        setCARType(x, BIN_START);
        Push(c1, x);
        any y = consName(CONTEXT_PTR, n, Nil);
        setCARType(y, BIN);
        c1.car->car = y;
        return Pop(c1);
    }
}
