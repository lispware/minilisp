#include <lisp.h>

static void dumpHeap(heap *h, FILE *fp)
{
    if(!h) return;

    dumpHeap(h->next, fp);
    fprintf(fp, "HEAP\n");
    for(int i=0; i < CELLS; i++)
    {
        any c = &(h->cells[i]);
        fprintf(fp, "%p %p %p %p\n", &c->car, c->car, c->cdr, c->meta.ptr);
    }
}

static int CTR;

void dumpMemory(Context *CONTEXT_PTR)
{
    char fileName[20];
    sprintf(fileName, "Memory_%03d.dump", CTR++);
    FILE *fp = fopen(fileName, "w");
    fprintf(fp, "MEM\n");

    for (int i = 1; i < MEMS; i++)
    {
        any cell = (any)(CONTEXT_PTR->Mem + i);
        fprintf(fp, "%p %p %p %p\n", cell, cell->car, cell->cdr, cell->meta.ptr);
    }

    fprintf(fp, "---------------------------\n");

    dumpHeap(CONTEXT_PTR->Heaps, fp);

    fclose(fp);
}
