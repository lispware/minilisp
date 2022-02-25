#include <lisp.h>
#include <tommath.h>

// (/ 'num ..) -> num
any doDiv(Context *CONTEXT_PTR, any ex)
{
    mp_err _mp_error;
    cell c1, c2;
    any x, y, z;

    x = cdr(ex);
    if (isNil(data(c1) = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex, data(c1));

    Push(c1, copyNum(CONTEXT_PTR, data(c1)));

    x = cdr(x);

    Push(c2, EVAL(CONTEXT_PTR, car(x)));
    if (isNil(data(c2)))
    {
        drop(c1);
        return Nil;
    }

    NeedNum(ex, data(c2));

    data(c2) = copyNum(CONTEXT_PTR, data(c2));
    mp_int *c = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(c);
    mp_int *d = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(d);
    _mp_error = mp_div(num(data(c1)), num(data(c2)), c, d);

    NewNumber(ext1, c, r1);
    data(c1) = r1;

    NewNumber(ext2, d, r2);
    data(c2) = r2;

    any result = cons(CONTEXT_PTR, data(c1), cons(CONTEXT_PTR, data(c2), Nil));

    Pop(c1);
    return result;
}
