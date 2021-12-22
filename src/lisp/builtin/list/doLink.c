#include <lisp.h>

// (link 'any ..) -> any
any doLink(Context *CONTEXT_PTR, any x)
{
    any y;

    if (!CONTEXT_PTR->Env.make)
    {
        makeError(x);
    }
    x = cdr(x);
    do
    {
        y = EVAL(CONTEXT_PTR, car(x));
        any r = *CONTEXT_PTR->Env.make = cons(CONTEXT_PTR, y, Nil);
        setCARType(r, PTR_CELL);
        CONTEXT_PTR->Env.make = &cdr(r);
    }
    while (!isNil(x = cdr(x)));
    return y;
}
