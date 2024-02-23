#include "pico.h"

#define PACK(__M, __R) any __R; { \
    word __r = (word)__M; \
    word allOnes = -1; \
    uword __mask = ((uword)allOnes >> (BITS / 2)); \
    word __low = __r & __mask; \
    word __high = (__r >> (BITS / 2))  & __mask; \
    __R = cons(box(__high), box(__low)); }

#define UNPACK(__M, __R) uword __R; {\
    uword __H = unBox(car(__M));\
    uword __L = unBox(cdr(__M));\
    __R = (__H << (BITS / 2)) | __L; }


any LISP_BN(any ex)
{
    any x = ex;
    x = cdr(x);
    any p1 = EVAL(car(x));
    x = cdr(x);
    any p2 = EVAL(car(x));
    word radix = 10;

    bf_t *NUM = (bf_t*)calloc(sizeof(bf_t), 1);
    bf_init(&bf_ctx, NUM);

    if (!isNil(p2))
    {
        radix = unBox(p2);
    }

    if (isNum(p1))
    {
        if (bf_set_si(NUM, unBox(p1)))
        {
            goto FAIL;
        }

        PACK(NUM, R);
        return R;
    }

    if (isSymb(p1))
    {
        char *number = (char *)calloc(bufSize(p1), 1);
        bufString(p1, number);
        bf_atof(NUM, number, NULL, radix, BF_PREC_INF, BF_RNDF);
        free(number);
        PACK(NUM, R);
        return R;
    }

FAIL:
    bf_delete(NUM);
    free(NUM);
    return Nil;
}

any LISP_BN_Add(any ex)
{
    any x, y;
    word n;

    x = cdr(ex);
    if (isNil(y = EVAL(car(x))))
        return Nil;

    bf_t *SUM = (bf_t*)calloc(sizeof(bf_t), 1);
    bf_init(&bf_ctx, SUM);

    UNPACK(y, _n);
    bf_set(SUM, (bf_t*)_n);

    while (isCell(x = cdr(x))) {
        if (isNil(y = EVAL(car(x))))
            return Nil;
            UNPACK(y, __n);
            bf_add(SUM, SUM,(bf_t*)__n, BF_PREC_INF, BF_RNDN);
    }

    PACK(SUM, R);
    return R;
}

any LISP_BN_delete(any ex)
{
    any x, y;
    word n;

    x = cdr(ex);
    if (isNil(y = EVAL(car(x))))
        return Nil;

    UNPACK(y, _n);
    //printf("DELETEING %s ", bf_ftoa(NULL, _n, 10, 10, BF_RNDF));
    bf_delete((bf_t*)_n);

    while (isCell(x = cdr(x))) {
        if (isNil(y = EVAL(car(x))))
            return Nil;
        UNPACK(y, __n);
        //printf("DELETEING %s ", bf_ftoa(NULL, __n, 10, 10, BF_RNDF));
        bf_delete((bf_t*)__n);
    }

    printf("\n");

    return Nil;
}

any LISP_BN_ToA(any ex)
{
    any x = ex;
    x = cdr(x);
    any p1 = EVAL(car(x));
    x = cdr(x);
    any p2 = EVAL(car(x));
    x = cdr(x);
    any p3 = EVAL(car(x));

    word nDigits = 10;
    word radix = 10;

    UNPACK(p1, _n);
    bf_t *n = (bf_t*)_n;

    if (!isNil(p2))
    {
        nDigits = unBox(p2);
    }

    if (!isNil(p3))
    {
        radix = unBox(p3);
    }

    char *number = bf_ftoa(NULL, n, radix, nDigits, BF_RNDF);
    any ret = mkStr(number);
    free(number);

    return ret;
}
