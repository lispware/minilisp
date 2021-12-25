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
    else if (type == FUNC || type == BIN)
    {
        toCell->car = fromCell->car;
    }
    else // PTR_CELL
    {
        uword *temp2 = makeptr(fromCell->car)->cdr;
        toCell->car = setPtrType((any)temp2[1], PTR_CELL);
    }

    if (cdrOfFromCell != 0)
    {
        any x = makeptr(cdrOfFromCell);
        uword *temp2 = x->cdr;
        toCell->cdr = setPtrType((any)temp2[1], type);
    }
}
