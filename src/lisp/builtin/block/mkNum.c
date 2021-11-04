#include <lisp.h>
#include <tommath.h>

any mkNum(Context *CONTEXT_PTR, word n)
{
    mp_err _mp_error;

    mp_int *BIGNUM = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(BIGNUM);
    mp_set(BIGNUM, n);

    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)BIGNUM;
    r->meta.type.parts[0] = NUM;
    return r;
}
