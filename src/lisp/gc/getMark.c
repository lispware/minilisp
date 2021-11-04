#include <lisp.h>

int getMark(any cell)
{
    return cell->meta.type.parts[3];
}
