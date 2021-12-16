#include <lisp.h>

any consSym(Context *CONTEXT_PTR, any val, any w)
{
    cell *p;

    if (!(p = CONTEXT_PTR->Avail)) {
        cell c1;

        if (!val)
            gc(CONTEXT_PTR, CELLS);
        else {
            Push(c1,val);
            gc(CONTEXT_PTR, CELLS);
            drop(c1);
        }
        p = CONTEXT_PTR->Avail;
    }
    CONTEXT_PTR->Avail = car(p);
    cdr(p) = val ? val : p;
    car(p) = (any)w;
    return p;
}
