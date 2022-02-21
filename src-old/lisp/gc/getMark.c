#include <lisp.h>

int getMark(any cell)
{
    //return makeptr(cell)->meta.type.parts[3];
    return ((any)((((uword)cdr(cell))) & ~4));
}
