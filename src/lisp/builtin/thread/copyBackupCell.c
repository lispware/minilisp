#include "thread.h"

void copyBackupCell(cell *fromCell, cell * toCell)
{
    toCell->meta.type = fromCell->meta.type;
    fromCell->meta.ptr = toCell;
}
