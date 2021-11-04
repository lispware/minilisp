#include <lisp.h>


uword getHeapSize(Context *CONTEXT_PTR)
{
    int size = 0;
    int sizeFree = 0;
    heap *h = CONTEXT_PTR->Heaps;
    do
    {
        any p = h->cells + CELLS-1;
        do
        {
            size++;
        }
        while (--p >= h->cells);
    } while (h = h->next);

    any p = CONTEXT_PTR->Avail;
    while (p)
    {
        sizeFree++;
        p = car(p);
    }

    printf("MEM SIZE = %d FREE = %d\n", size, sizeFree);
    return size;
}
