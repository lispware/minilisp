#include "thread.h"
#include <tommath.h>

void copyFixupCell(Context *From, Context *To, cell *fromCell, cell * toCell)
{
    uword *temp = (uword*)fromCell->cdr;
    any cdrOfFromCell = (any)temp[0];
    CellPartType type = temp[0] & 7;

    if (type == EXT)
    {
        external *e = (external*)fromCell->car;
        if (e) toCell->car = e->copy(From, e);
        else toCell->car = fromCell->car;
    }
    else if (!cdrOfFromCell)
    {
        //any c = fromCell->car;
        //if (c)
        //{
        //    uword *temp2 = makeptr(c)->cdr;
        //    toCell->car = (any)temp2[1];
        //}
        toCell->car = NULL;
    }
    else if (type == FUNC || type == BIN)
    {
        toCell->car = fromCell->car;
    }
    else // PTR_CELL
    {
        uword *temp2 = makeptr(fromCell->car)->cdr;
        toCell->car = (any)temp2[1];
    }

    if (cdrOfFromCell != 0)
    {
        any x = makeptr(cdrOfFromCell);
        uword *temp2 = x->cdr;
        toCell->cdr = setPtrType((any)temp2[1], type);
    }
}
