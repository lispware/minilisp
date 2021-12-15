#include <lisp.h>
#include <tommath.h>

any doSub(Context *CONTEXT_PTR, any ex)
{
    any x;
    cell c1, c2;
    mp_err _mp_error;

    x = cdr(ex);
    if (isNil(data(c1) = EVAL(CONTEXT_PTR, car(x))))
    {
        mp_int *id = (mp_int*)malloc(sizeof(mp_int));
        _mp_error = mp_init(id); // TODO handle the errors appropriately
        mp_set_i32(id, 0);

        NewNumber(ext, id, idr);
        return idr;
    }

    NeedNum(ex, data(c1));

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(n); // TODO handle the errors appropriately
    _mp_error = mp_copy(num(data(c1)), n);

    NewNumber(ext, n, r);
    Push(c1, r);

    while (Nil != (x = cdr(x)))
    {
        Push(c2, EVAL(CONTEXT_PTR, car(x)));
        if (isNil(data(c2)))
        {
            drop(c1);
            return Nil;
        }

        NeedNum(ex,data(c2));
        mp_int *m = num(data(c2));
        _mp_error = mp_sub(n, m, n);

        drop(c2);
    }

    return Pop(c1);
}
