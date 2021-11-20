#include <lisp.h>
#include <tommath.h>

void pack(Context *CONTEXT_PTR, any x, int *i, uword *p, any *q, cell *cp)
{
    int c, j;
    uword w;
    mp_err _mp_error;

    if (x != Nil && getCARType(x) == PTR_CELL)
    {
        do
        {
            if (getCARType(x) == PTR_CELL)
            {
                pack(CONTEXT_PTR, car(x), i, p, q, cp);
            }
            else
            {
                pack(CONTEXT_PTR, x, i, p, q, cp);
            }
        }
        while (Nil != (x = cdr(x)));
    }
    if (isNum(x))
    {
        int len;
        _mp_error = mp_radix_size((mp_int*)x->car, 10, &len);
        char *buf = (char*)malloc(len);

        _mp_error = mp_to_radix((mp_int*)x->car, buf, len, NULL, 10);
        char *b = buf;

        do
            putByte(CONTEXT_PTR, *b++, i, p, q, cp);
        while (*b);
        free(buf);
    }
    else if (!isNil(x))
    {
        for (x = x, c = getByte1(CONTEXT_PTR, &j, &w, &x); c; c = getByte(CONTEXT_PTR,&j, &w, &x))
        {
            putByte(CONTEXT_PTR, c, i, p, q, cp);
        }
    }
}
