#include <lisp.h>

any cons(Context *CONTEXT_PTR, any x, any y)
{
    cell *p;

    if (!(p = CONTEXT_PTR->Avail))
    {
        cell c1, c2;

        Push(c1,x);
        Push(c2,y);
        gc(CONTEXT_PTR, CELLS);
        drop(c1);
        p = CONTEXT_PTR->Avail;
    }
    CONTEXT_PTR->Avail = p->car;
    p->car = x;
    p->cdr = y;
    setCARType(p, PTR_CELL);
    return p;
}
