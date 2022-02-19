#include <lisp.h>

static int INDEX;

extern int CONSCTR;

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
#if 1
    //if (name[0] != 't' || name[1] != 'h') return;
    //if (CONSCTR < 1000) return;

    //if (THETHREAD != pthread_self() || name[0]!='t' || name[1] != '0') return;

    return;

    char fileName[40];
    sprintf(fileName, "%05d_%s_%d.dump",INDEX++, name, CONTEXT_PTR->THREAD_COUNT);
    FILE *fp = fopen(fileName, "w");
    fprintf(fp, "MEM %p\n", CONTEXT_PTR->Avail);

    for (int i = 0; i < MEMS; i++)
    {
        any cell = (any)(CONTEXT_PTR->Mem + i);
        fprintf(fp, "%014p %014p %014p %014p\n", &cell->car, cell->car, cell->cdr, cell->meta.ptr);
    }

    fprintf(fp, "---------------------------\n");

    dumpHeap(CONTEXT_PTR->Heaps, fp);

    fclose(fp);
#endif
}
