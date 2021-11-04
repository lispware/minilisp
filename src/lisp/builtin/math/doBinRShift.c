#include <lisp.h>
#include <tommath.h>

// (>> 'num ..) -> num
any doBinRShift(Context *CONTEXT_PTR, any ex)
{
    mp_err _mp_error;
    any x, y;
    word s = 1;

    x = cdr(ex);
    any p1 = EVAL(CONTEXT_PTR, car(x));

    if (isNil(p1) || !isNum(p1)) return Nil;

    s = mp_get_i32((mp_int*)p1->car);

    x = cdr(x);
    any p2 = EVAL(CONTEXT_PTR, car(x));

    if (isNil(p2) || !isNum(p2)) return Nil;

    mp_int *m = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(m);
    if (s >= 0)
    {
        _mp_error = mp_div_2d((mp_int*)p2->car, s, m, NULL);
    }
    else
    {
        s *= -1;
        _mp_error = mp_mul_2d((mp_int*)p2->car, s, m);
    }

    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)m;
    r->meta.type.parts[0] = NUM;
    return r;
}
