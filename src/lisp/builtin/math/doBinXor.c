#include <lisp.h>
#include <tommath.h>

// (x| 'num ..) -> num
any doBinXor(Context *CONTEXT_PTR, any ex)
{
    return Nil;
#if 0
    mp_err _mp_error;
    any x, y;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(n);
    _mp_error = mp_copy((mp_int*)y->car, n);

    while (Nil != (x = cdr(x)))
    {
        if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
            return Nil;
        NeedNum(ex,y);
        mp_int *m = (mp_int*)y->car;
        _mp_error = mp_xor(n, m, n);

    }

    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)n;
    r->meta.type.parts[0] = NUM;
    return r;
#endif
}


