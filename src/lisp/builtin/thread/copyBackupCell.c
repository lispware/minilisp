#include "thread.h"

void copyBackupCell(cell *fromCell, cell * toCell)
{
    uword  *temp = (uword*)malloc(sizeof(uword*) * 2);
    temp[0] = fromCell->cdr;
    temp[1] = toCell;
    fromCell->cdr = temp;
}
