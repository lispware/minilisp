#include <lisp.h>

any popSym(Context *CONTEXT_PTR, int i, uword n, any q, cell *cp)
{
    if (q)
    {
        cdr(q) = consName(CONTEXT_PTR, n, Nil);
        setCARType(q, BIN);
        setCARType(cdr(q), BIN);
        return Pop(*cp);
    }
    else
    {
        any x = consSym(CONTEXT_PTR, NULL, Nil);
        setCARType(x, BIN_START);
        Push(*cp, x);
        any y = consName(CONTEXT_PTR, n, Nil);
        setCARType(y, BIN);
        car(car(cp)) = y;
        return Pop(*cp);
    }
}
