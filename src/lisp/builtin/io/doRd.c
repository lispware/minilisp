#include <lisp.h>
#include <tommath.h>

any doRd(Context *CONTEXT_PTR, any ex)
{
    return Nil;
#if 0
    mp_err _mp_error;
    any params = cdr(ex);
    any p1 = car(params);
    any p2 = cadr(params);
    any p3 = caddr(params);

    p1 = EVAL(CONTEXT_PTR, p1);
    if (!isNum(p1)) return Nil;
    size_t count = mp_get_i32((mp_int*)p1->car);

    unsigned char *buf = (char *)malloc(count);
    //if (fread(buf, 1, count, CONTEXT_PTR->InFile) == 0)
    //{
    //    // TODO - EOF probably or some other error
    //    return Nil;
    //}

    for(int i = 0;i < count; i++)
    {
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        if (CONTEXT_PTR->Chr < 0 ) break;
        buf[i] = CONTEXT_PTR->Chr;
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

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(n); // TODO handle the errors appropriately

    _mp_error = mp_unpack(n, count, order, 1, endianess, 0, (const void *)buf);
    free(buf);

    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)n;
    r->meta.type.parts[0] = NUM;
    return r;
#endif
}
