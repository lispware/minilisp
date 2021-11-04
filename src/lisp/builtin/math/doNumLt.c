#include <lisp.h>
#include <tommath.h>

// (+ 'num ..) -> num
any doNumLt(Context *CONTEXT_PTR, any ex)
{
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
        if (MP_LT != mp_cmp(n, m))
        {
            mp_clear(n);
            free(n);
            return Nil;
        }

    }

    mp_clear(n);
    free(n);
    return T;
}

