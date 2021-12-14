#include <lisp.h>
#include <tommath.h>

// (rand 'num ..) -> num
any doRandom(Context *CONTEXT_PTR, any ex)
{
    any x, y;
    uword s = 32;
    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    mp_err _mp_error = mp_init(n);

    x = cdr(ex);
    if (!isNil(y = EVAL(CONTEXT_PTR, car(x))))
    {
        _mp_error = mp_copy(num(y), n);
        s = mp_get_i32(n);
    }

    _mp_error = mp_rand(n, s);

    NewExtNum(ext, n);

    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)ext;
    r->meta.type.parts[0] = EXT;
    return r;
}
