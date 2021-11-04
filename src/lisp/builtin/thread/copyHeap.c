#include <lisp.h>
#include "thread.h"

void copyHeap(Context *From, Context *To)
{
    for (int i = 0;i < From->HeapCount; i++)
    {
        heapAlloc(To);
    }
    To->Mem=(any)calloc(1, sizeof(cell)*MEMS);

    /////////////////////////////////////////////////////
    //dumpMem(From, "DEBUG_HEAP0.txt");
    //dumpMem(To, "DEBUG_COPY0.txt");
    heap *from = From->Heaps;
    heap *to = To->Heaps;
    for(int i = 0; i < MEMS; i++)
    {
        any fromCell = &(From->Mem[i]);
        any toCell = (any)(To->Mem + i);
        copyBackupCell(fromCell, toCell);
    }
    while(from)
    {
        for(int j=0; j < CELLS; j++)
        {
            cell *fromCell = &from->cells[j];
            cell *toCell = &to->cells[j];
            copyBackupCell(fromCell, toCell);
        }

        from=from->next;
        to=to->next;
    }
    //dumpMem(From, "DEBUG_HEAP1.txt");
    //dumpMem(To, "DEBUG_COPY1.txt");

    /////////////////////////////////////////////////////
    from = From->Heaps;
    to = To->Heaps;
    for(int i = 0; i < MEMS; i++)
    {
        cell *fromCell = From->Mem + i;
        cell *toCell = To->Mem + i;
        copyFixupCell(From, To, fromCell, toCell);
    }
    while(from)
    {
        for(int j=0; j < CELLS; j++)
        {
            any fromCell = &from->cells[j];
            any toCell = &to->cells[j];
            copyFixupCell(From, To, fromCell, toCell);

        }

        from=from->next;
        to=to->next;
    }

    /////////////////////////////////////////////////////
    from = From->Heaps;
    to = To->Heaps;
    for(int i = 0; i < MEMS; i++)
    {
        cell *fromCell = From->Mem + i;
        cell *toCell = To->Mem + i;
        copyRestoreCell(From, To, fromCell, toCell);
    }
    while(from)
    {
        for(int j=0; j < CELLS; j++)
        {
            any fromCell = &from->cells[j];
            any toCell = &to->cells[j];
            copyRestoreCell(From, To, fromCell, toCell);
        }

        from=from->next;
        to=to->next;
    }

    //dumpMem(From, "DEBUG_HEAP2.txt");
    //dumpMem(To, "DEBUG_COPY2.txt");
}
