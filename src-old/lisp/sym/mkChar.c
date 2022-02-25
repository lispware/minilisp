#include <lisp.h>

any mkChar(Context *CONTEXT_PTR, int c)
{
    cell c1;
    any r = cons(CONTEXT_PTR, Nil, Nil);
    Push (c1, r);
    car(data(c1)) = cons(CONTEXT_PTR, Nil, Nil);
    cdr(data(c1)) = Nil;
    car(car(data(c1))) = (any)(uword)c;
    car(cdr(data(c1))) = Nil;

    setCARType(data(c1), BIN_START);
    setCARType(car(data(c1)), BIN);

    return Pop(c1);
}
