#include <lisp.h>
#include <tommath.h>

any copyNum(Context *CONTEXT_PTR, any n)
{
    mp_err _mp_error;

    if (!isNum(n)) return Nil;

    mp_int *BIGNUM = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(BIGNUM);
    _mp_error = mp_copy((mp_int*)n->car, BIGNUM);

    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)BIGNUM;
    r->meta.type.parts[0] = NUM;
    return r;
}
