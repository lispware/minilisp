#include <lisp.h>
#include <tommath.h>

any indx(Context *CONTEXT_PTR, any x, any y)
{
    any z = y;

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    mp_err _mp_error = mp_init(n); // TODO handle the errors appropriately
    mp_set(n, 1);

    while (!isNil(y))
    {
        if (0 == equal(CONTEXT_PTR, x, car(y)))
        {
            NewNumber(ext, n, r);
            return r;
        }
        _mp_error = mp_incr(n);
        if (z == (y = cdr(y)))
            return Nil;
    }
    return Nil;
}
