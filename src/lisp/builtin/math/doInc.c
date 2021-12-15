#include <lisp.h>
#include <tommath.h>

// (inc 'num ..) -> num
any doInc(Context *CONTEXT_PTR, any ex)
{
    any x;
    cell c1, c2;
    mp_err _mp_error;

    x = cdr(ex);
    if (isNil(data(c1) = EVAL(CONTEXT_PTR, car(x))))
        return Nil;

    NeedNum(ex, data(c1));

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(n); // TODO handle the errors appropriately
    _mp_error = mp_add_d(num(data(c1)), 1, n);

    NewNumber(ext, n, r);

    return r;
}
