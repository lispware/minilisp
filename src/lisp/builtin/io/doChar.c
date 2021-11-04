#include <lisp.h>
#include <tommath.h>

// (char) -> sym
// (char 'num) -> sym
// (char 'sym) -> num
any doChar(Context *CONTEXT_PTR, any ex)
{
    any x = cdr(ex);

    if (x == Nil)
    {
        if (!CONTEXT_PTR->Chr)
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
        }

        x = CONTEXT_PTR->Chr < 0 ? Nil : mkChar(CONTEXT_PTR, CONTEXT_PTR->Chr);
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        return x;
    }

    x = EVAL(CONTEXT_PTR, car(x));

    if (isNum(x))
    {
        return mkChar(CONTEXT_PTR, mp_get_i32((mp_int*)x->car));
    }

    CellPartType t = getCARType(x);
    if (t == TXT || t == BIN_START)
    {

        mp_int *n = (mp_int*)malloc(sizeof(mp_int));
        mp_err _mp_error = mp_init(n); // TODO handle the errors appropriately
        mp_set(n, firstByte(CONTEXT_PTR, x));
        any r = cons(CONTEXT_PTR, Nil, Nil);
        r->car = (any)n;
        r->meta.type.parts[0] = NUM;
        return r;
    }
    return Nil;
}
