#include <lisp.h>

int getByte(Context *CONTEXT_PTR, int *i, uword *p, any *q)
{
    int c;

    if (*i == 0)
    {
        if (!*q || isNil(*q))
        {
            return 0;
        }
        else
        {
            *i = BITS,  *p = (uword)(car(*q)),  *q = cdr(*q);
        }
    }
    c = *p & 0xff,  *p >>= 8;
    if (*i >= 8)
        *i -= 8;
    else if (isNum(*q))
    {
        *p = (uword)*q >> 2,  *q = NULL;
        c |= *p << *i;
        *p >>= 8 - *i;
        *i += BITS-9;
    }
    else
    {
        *p = (uword)tail(*q),  *q = val(*q);
        c |= *p << *i;
        *p >>= 8 - *i;
        *i += BITS-8;
    }
    c &= 0xff;

    return c;
}
