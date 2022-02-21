#include <lisp.h>

void setMark(any cell, int m)
{
    //makeptr(cell)->meta.type.parts[3] = m;
    if (m)
    {
        cdr(cell) = ((any)((((uword)cdr(cell))) | 4));
    }
    else
    {
        cdr(cell) = ((any)((((uword)cdr(cell))) & ~4));
    }
}
