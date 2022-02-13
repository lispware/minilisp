#include <lisp.h>
#include "thread.h"


void RestoreStack(Context *From, Context *To)
{
    any stackptr = From->Env.stack;
    if (!stackptr) return;
    To->Env.stack = (any)calloc(sizeof(cell), 1);
    any tostackptr = To->Env.stack;

    while (stackptr)
    {
        any fromCell = car(stackptr);
        any toCell = car(tostackptr) = (any)calloc(sizeof(cell), 1);

        uword *temp23 = (uword*)fromCell->cdr;
        uword *temp = (uword*)cdr(fromCell);
        any cdrOfFromCell = (any)temp[0];
        CellPartType type = temp[0] & 3;

        any c = car(fromCell);
        if (c)
        {
            uword *temp2 = makeptr(c)->cdr;
            toCell->car = (any)temp2[1];
        }

        any x = makeptr(cdrOfFromCell);
        uword *temp2 = x->cdr;
        toCell->cdr = setPtrType((any)temp2[1], type);


        stackptr = cdr(stackptr);
        if (stackptr)
        {
            cdr(tostackptr) = (any)calloc(sizeof(cell), 1);
            tostackptr = cdr(tostackptr);
        }
        else
        {
            cdr(tostackptr) = NULL;
        }
    }
}

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

    dumpMemory(From, "t1");
    dumpMemory(To, "t1");

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
    


    // COPY STACK
    RestoreStack(From, To);


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
