#include <lisp.h>
#include <tommath.h>

external * copyExtNum(Context *CONTEXT_PTR, external *ext)
{
    mp_err _mp_error;

    if (ext->type != EXT_NUM)
    {
        fprintf(stderr, "Not a number\n");
        exit(0);
    }

    mp_int *BIGNUM = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(BIGNUM);
    _mp_error = mp_copy((mp_int*)ext->pointer, BIGNUM);

    external *e = (external *)malloc(sizeof(external));
    e->type = EXT_NUM;
    e->release = releaseExtNum;
    e->print = printExtNum;
    e->copy = copyExtNum;
    e->equal = equalExtNum;
    e->pointer = (void*)(BIGNUM);

    return e;
}
