#include "thread.h"

void copyRestoreCell(Context *From, Context *To, cell *fromCell, cell *toCell)
{
    if (fromCell== From->Avail)
    {
        To->Avail = toCell;
    }
    if (fromCell== From->Intern[0])
    {
        To->Intern[0] = toCell;
    }
    if (fromCell== From->Intern[1])
    {
        To->Intern[1] = toCell;
    }
    if (fromCell== From->Transient[0])
    {
        To->Transient[0] = toCell;
    }
    if (fromCell== From->Transient[1])
    {
        To->Transient[1] = toCell;
    }
    if (fromCell== To->Code)
    {
        To->Code = toCell;
    }

    fromCell->meta.type = toCell->meta.type;
}
