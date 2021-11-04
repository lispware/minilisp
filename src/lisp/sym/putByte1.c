#include <lisp.h>

void putByte1(int c, int *i, uword *p, any *q)
{
    *p = c & 0xff;
    *i = 8;
    *q = NULL;
}
