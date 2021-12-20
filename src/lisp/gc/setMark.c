#include <lisp.h>

void setMark(any cell, int m)
{
    makeptr(cell)->meta.type.parts[3] = m;
}
