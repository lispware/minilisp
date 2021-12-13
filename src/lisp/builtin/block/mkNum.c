#include <lisp.h>
#include <tommath.h>

any mkNum(Context *CONTEXT_PTR, word n)
{
    mp_err _mp_error;

    mp_int *BIGNUM = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(BIGNUM);
    mp_set(BIGNUM, n);

    NewExtNum(ext, BIGNUM);

    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)ext;
    r->meta.type.parts[0] = EXT_NUM;
    return r;
}
