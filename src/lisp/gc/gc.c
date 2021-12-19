#include <lisp.h>
#include <tommath.h>

void gc(Context *CONTEXT_PTR, word c)
{
    any p;
    heap *h;

    dumpMemory(CONTEXT_PTR);
    markAll(CONTEXT_PTR);
    dumpMemory(CONTEXT_PTR);

    /* Sweep */
    CONTEXT_PTR->Avail = NULL;
    h = CONTEXT_PTR->Heaps;
    if (c)
    {
        do
        {
            p = h->cells + CELLS-1;
            do
            {
                if (!getMark(p))
                {
                    if (GetType(p) == EXT)
                    {
                        external *e = (external*)car(p);
                        if (e) e->release(e);
                    }
                    memset(p, 0, sizeof(cell));
                    car(p) = CONTEXT_PTR->Avail;
                    CONTEXT_PTR->Avail = p;
                    --c;
                }
                setMark(p, 0);
            }
            while (--p >= h->cells);
        } while (h = h->next);


        while (c >= 0)
        {
            heapAlloc(CONTEXT_PTR),  c -= CELLS;
        }
    }

    return;
}
