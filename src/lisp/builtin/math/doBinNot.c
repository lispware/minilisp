#include <lisp.h>
#include <tommath.h>

// (& 'num ..) -> num
any doBinNot(Context *CONTEXT_PTR, any ex)
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

    _mp_error = mp_complement(n, n);

    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)n;
    r->meta.type.parts[0] = NUM;
    return r;
}


