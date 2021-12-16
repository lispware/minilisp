#include "thread.h"

void *thread_func(void *arg)
{
    Context *CONTEXT_PTR = arg;

    EVAL(CONTEXT_PTR, CONTEXT_PTR->Code);
    heap *h = CONTEXT_PTR->Heaps;

    while (h)
    {
        heap *x = h;
        h = h->next;
        free(x);
    }

    free(CONTEXT_PTR->Mem);
    free(CONTEXT_PTR);

    return NULL;
}

any doThread(Context *CONTEXT_PTR_ORIG, any x)
{
    Context *CONTEXT_PTR = CONTEXT_PTR_ORIG;

    CONTEXT_PTR = (Context*)calloc(1, sizeof(Context));
    CONTEXT_PTR->InFile = stdin, CONTEXT_PTR->Env.get = getStdin;
    CONTEXT_PTR->OutFile = stdout, CONTEXT_PTR->Env.put = putStdout;
    CONTEXT_PTR->Code = caddr(x);
    CONTEXT_PTR_ORIG->Code = CONTEXT_PTR->Code;

    CONTEXT_PTR->ApplyArgs = Nil; //cons(CONTEXT_PTR, cons(CONTEXT_PTR, consSym(CONTEXT_PTR, Nil, 0), Nil), Nil);
    CONTEXT_PTR->ApplyBody = Nil; //cons(CONTEXT_PTR, Nil, Nil);

    copyHeap(CONTEXT_PTR_ORIG, CONTEXT_PTR);
    CONTEXT_PTR->Mem[0].car = CONTEXT_PTR->Mem[0].cdr; // TODO - should find a better place for this
    if (!CONTEXT_PTR_ORIG->Avail) CONTEXT_PTR->Avail = 0;

    // Clear out the items that need to be moved to the new thread
    x = cadr(x);
    any m = EVAL(CONTEXT_PTR_ORIG, car(x));
    while (GetType(m) == EXT)
    {
        car(m) = NULL;
        x = cdr(x);
        m = EVAL(CONTEXT_PTR_ORIG, car(x));
    }

    plt_thread_start(CONTEXT_PTR, thread_func, 0); //TODO - passing nowait seems to not work

    CONTEXT_PTR = CONTEXT_PTR_ORIG;
    return Nil;
}
