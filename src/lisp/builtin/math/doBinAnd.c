#include <lisp.h>
#include <tommath.h>

// (& 'num ..) -> num
any doBinAnd(Context *CONTEXT_PTR, any ex)
{
    any x, y;
    mp_err _mp_error;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(n);
    _mp_error = mp_copy(num(y), n);

    while (Nil != (x = cdr(x)))
    {
        if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
            return Nil;
        NeedNum(ex,y);
        mp_int *m = num(y);
        _mp_error = mp_and(n, m, n);

    }

    NewNumber(ext, n, r);
    return r;
}
