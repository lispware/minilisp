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
        any c = cons(CONTEXT_PTR, y, Nil);

        //c = (any)((((uword)c) & ~7) | 1); /// Set the CDR type to PTR_CELL
        setPtrType(c, PTR_CELL);

        *CONTEXT_PTR->Env.make = c;
        
        CONTEXT_PTR->Env.make = &cdr(c);

    }
    while (!isNil(x = cdr(x)));
    return y;
}
