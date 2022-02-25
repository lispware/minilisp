#include <lisp.h>

int getByte1(Context *CONTEXT_PTR, int *i, uword *p, any *q)
{
    int c;

    if (isSym(*q))
    {
        (*q)=car(*q);
        *i = BITS, *p = (uword)(car(*q)) , *q = (cdr(*q));
    }
    else
    {
        giveup("Cant getByte");
    }

    c = *p & 0xff, *p >>= 8, *i -= 8;

    return c;
}
