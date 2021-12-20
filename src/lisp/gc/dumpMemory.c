#include <lisp.h>

static int INDEX;

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

void dumpMemory(Context *CONTEXT_PTR, char *name)
{
    char fileName[20];
    sprintf(fileName, "%05d_%s.dump",INDEX++, name);
    FILE *fp = fopen(fileName, "w");
    fprintf(fp, "MEM\n");

    for (int i = 0; i < MEMS; i++)
    {
        any cell = (any)(CONTEXT_PTR->Mem + i);
        fprintf(fp, "%014p %014p %014p %014p\n", &cell->car, cell->car, cell->cdr, cell->meta.ptr);
    }

    fprintf(fp, "---------------------------\n");

    dumpHeap(CONTEXT_PTR->Heaps, fp);

    fclose(fp);
}
