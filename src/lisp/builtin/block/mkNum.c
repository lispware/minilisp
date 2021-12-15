#include <lisp.h>
#include <tommath.h>

any mkNum(Context *CONTEXT_PTR, word n)
{
    mp_err _mp_error;

    mp_int *BIGNUM = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(BIGNUM);
    mp_set(BIGNUM, n);

    NewNumber(ext, BIGNUM, r);
    return r;
}
