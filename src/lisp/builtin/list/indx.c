#include <lisp.h>
#include <tommath.h>

any indx(Context *CONTEXT_PTR, any x, any y)
{
    return Nil;
#if 0
    any z = y;

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    mp_err _mp_error = mp_init(n); // TODO handle the errors appropriately
    mp_set(n, 1);

    while (y != Nil)
    {
        if (0 == equal(CONTEXT_PTR, x, car(y)))
        {
            any r = cons(CONTEXT_PTR, Nil, Nil);
            r->car = (any)n;
            r->meta.type.parts[0] = NUM;
            return r;
        }
        _mp_error = mp_incr(n);
        if (z == (y = cdr(y)))
            return Nil;
    }
    return Nil;
#endif
}
