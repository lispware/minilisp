#include <lisp.h>

int CONSCTR;
any cons(Context *CONTEXT_PTR, any x, any y)
{
    cell *p;

    CONSCTR++;
    dump("cons1");
    if (!(p = CONTEXT_PTR->Avail))
    {
        cell c1, c2;

        Push(c1,x);
        Push(c2,y);
        gc(CONTEXT_PTR, CELLS);
        drop(c1);
        p = CONTEXT_PTR->Avail;
        dump("cons2");
    }
    CONTEXT_PTR->Avail = car(p);
    car(p) = x;
    cdr(p) = y;
    setCARType(p, PTR_CELL);
    dump("cons3");

    return p;
}
