#include <lisp.h>

int getByte1(Context *CONTEXT_PTR, int *i, uword *p, any *q)
{
    int c;

    if (getCARType(*q) == BIN_START)
    {
        (*q)=(*q)->car;
        *i = BITS, *p = (uword)((*q)->car) , *q = ((*q)->cdr);
    }
    else
    {
        giveup("Cant getByte");
    }

    c = *p & 0xff, *p >>= 8, *i -= 8;

    return c;
}
