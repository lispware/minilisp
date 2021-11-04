#include <lisp.h>

void setMark(any cell, int m)
{
    cell->meta.type.parts[3] = m;
}
