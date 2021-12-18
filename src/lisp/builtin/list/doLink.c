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
        CONTEXT_PTR->Env.make = &cdr(*CONTEXT_PTR->Env.make = cons(CONTEXT_PTR, y, Nil));
    }
    while (!isNil(x = cdr(x)));
    return y;
}
