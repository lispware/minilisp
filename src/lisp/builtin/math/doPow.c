#include <lisp.h>
#include <tommath.h>

// (** 'num ..) -> num
any doPow(Context *CONTEXT_PTR, any ex)
{
    mp_err _mp_error;
    any x, y;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(n);
    _mp_error = mp_copy(num(y), n);

    while (!isNil(x = cdr(x)))
    {
        if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
            return Nil;
        NeedNum(ex,y);

        int m = mp_get_i32(num(y));
        _mp_error = mp_expt_u32(n, m, n);

    }

    NewNumber(ext, n, r);
    return r;
}
