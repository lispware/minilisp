#include <lisp.h>

any popSym(Context *CONTEXT_PTR, int i, uword n, any q, cell *cp)
{
    if (q)
    {
        q->cdr = consName(CONTEXT_PTR, n, Nil);
        setCARType(q->cdr, BIN);
        return Pop(*cp);
    }
    else
    {
        cell c1;
        any x = consSym(CONTEXT_PTR, NULL, Nil);
        setCARType(x, BIN_START);
        Push(c1, x);
        any y = consName(CONTEXT_PTR, n, Nil);
        setCARType(y, BIN);
        c1.car->car = y;
        return Pop(c1);
    }
}
