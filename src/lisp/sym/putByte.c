#include <lisp.h>

void putByte(Context *CONTEXT_PTR, int c, int *i, uword *p, any *q, cell *cp)
{
    c = c & 0xff;
    int d = 8;

    if (*i != BITS)
        *p |= (uword)c << *i;

    if (*i + d  > BITS)
    {
        if (*q)
        {
            any x = consName(CONTEXT_PTR, *p, Nil);
            setCARType(x, BIN);
            cdr(*q) = x;
            *q = x;
        }
        else
        {
            any x = consSym(CONTEXT_PTR, NULL, Nil);
            setCARType(x, BIN_START);
            Push(*cp, x);
            any y = consName(CONTEXT_PTR, *p, Nil);
            setCARType(y, BIN);
            car(car(cp)) = *q = y;
        }
        *p = c >> BITS - *i;
        *i -= BITS;
    }

    *i += d;
}
