#include <lisp.h>

// (chop 'any) -> lst
any doChop(Context *CONTEXT_PTR, any x)
{
    int c, i;
    uword w;
    char *h;
    cell c1, c2;
    any y = Nil;

    x = cdr(x);
    x = EVAL(CONTEXT_PTR, car(x));

    if(x==Nil) return Nil;

    c = getByte1(CONTEXT_PTR, &i, &w, &x);
    Push(c1, cons(CONTEXT_PTR, mkChar(CONTEXT_PTR, c), Nil));
    y = data(c1);
    while (c)
    {
        c = getByte(CONTEXT_PTR,&i, &w, &x);
        if (c)
        {
            y = cdr(y) = cons(CONTEXT_PTR, mkChar(CONTEXT_PTR, c), Nil);
        }
    }

    return Pop(c1);

}
