#include <lisp.h>

any mkChar(Context *CONTEXT_PTR, int c)
{
    cell c1;
    any r = cons(CONTEXT_PTR, Nil, Nil);
    Push (c1, r);
    data(c1)->car = cons(CONTEXT_PTR, Nil, Nil);
    data(c1)->cdr = Nil;
    data(c1)->car->car = (any)(uword)c;
    data(c1)->car->cdr = Nil;

    setCARType(data(c1), BIN_START);
    setCARType(data(c1)->car, BIN);

    return Pop(c1);
}
