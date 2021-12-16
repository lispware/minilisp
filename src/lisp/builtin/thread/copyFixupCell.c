#include "thread.h"
#include <tommath.h>

void copyFixupCell(Context *From, Context *To, cell *fromCell, cell * toCell)
{
    CellPartType carType;
    carType = GetType(toCell);

    if (carType == EXT)
    {
        external *e = (external*)car(fromCell);
        if (e) car(toCell) = (any)e->copy(From, e);
        else car(toCell) = car(fromCell);
    }
    else if ((carType == FUNC && car(fromCell) && cdr(fromCell)) || carType == BIN)
    {
        car(toCell) = car(fromCell);
    }
    else
    {
        if (car(fromCell) != 0)
        {
            car(toCell) = car(fromCell)->meta.ptr;
        }
        else
        {
            car(toCell) = car(fromCell);
        }
    }

    if (cdr(fromCell) != 0)
    {
        cdr(toCell) = cdr(fromCell)->meta.ptr;
    }
    else
    {
        cdr(toCell) = cdr(fromCell);
    }
}
