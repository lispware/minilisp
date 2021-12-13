#include <lisp.h>
#include <tommath.h>

void gc(Context *CONTEXT_PTR, word c)
{
    any p;
    heap *h;

    //doDump(CONTEXT_PTR, Nil);
    markAll(CONTEXT_PTR);
    //doDump(CONTEXT_PTR, Nil);

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
                    if (getCARType(p) == EXT)
                    {
                        external *e = (external*)p->car;
                        if (e) e->release(e);
                    }
                    memset(p, 0, sizeof(cell));
                    p->car = CONTEXT_PTR->Avail;
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
