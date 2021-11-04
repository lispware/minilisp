#include <lisp.h>
#include <tommath.h>

any doWr(Context *CONTEXT_PTR, any ex)
{
    mp_err _mp_error;
    any params = cdr(ex);
    any p1 = car(params);
    any p2 = cadr(params);
    any p3 = caddr(params);

    p1 = EVAL(CONTEXT_PTR, p1);
    if (!isNum(p1)) return Nil;
    size_t count = mp_pack_count((mp_int*)p1->car, 0, 1 );

    if (count == 0)
    {
        fputc(0, CONTEXT_PTR->OutFile);
    }


    mp_order order = MP_LSB_FIRST;
    p2 = EVAL(CONTEXT_PTR, p2);
    if (isNum(p2))
    {
        if (mp_get_i32((mp_int*)p2->car) == 1) order = MP_MSB_FIRST;
    }

    mp_endian endianess = MP_BIG_ENDIAN;
    p3 = EVAL(CONTEXT_PTR, p3);
    if (isNum(p3))
    {
        if (mp_get_i32((mp_int*)p3->car) == 1) endianess = MP_LITTLE_ENDIAN;
    }

    size_t written;
    unsigned char *buf = (char *)malloc(count);
    _mp_error = mp_pack((void *)buf, count, &written, order, 1, endianess, 0, (mp_int*)p1->car);

    fwrite(buf, 1, count, CONTEXT_PTR->OutFile);
    free(buf);

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(n); // TODO handle the errors appropriately
    mp_set(n, written);

    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)n;
    r->meta.type.parts[0] = NUM;
    return r;
}
