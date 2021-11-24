#include <lisp.h>

any mkChar(Context *CONTEXT_PTR, int c)
{
    cell c1;
    any r = cons(CONTEXT_PTR, Nil, Nil);
    Push (c1, r);
    r->car = cons(CONTEXT_PTR, Nil, Nil);
    r->cdr = Nil;
    r->car->car = (any)(uword)c;
    r->car->cdr = Nil;

    setCARType(r, BIN_START);
    setCARType(r->car, BIN);

    return Pop(c1);
}
