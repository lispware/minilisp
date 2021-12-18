#include <lisp.h>

// (setq var 'any ..) -> any
any doSetq(Context *CONTEXT_PTR, any ex)
{
    any x, y;

    x = cdr(ex);
    do
    {
        y = car(x),  x = cdr(x);
        NeedVar(ex,y);
        // CheckVar(ex,y); - TODO - what is this for?
        val(y) = EVAL(CONTEXT_PTR, car(x));
    }
    while (!isNil(x = cdr(x)));

    return val(y);
}
