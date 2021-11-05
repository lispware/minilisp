#include <lisp.h>
#include <tommath.h>

int eqList(Context *CONTEXT_PTR, any v1, any v2)
{
    while(v1 != Nil)
    {
        CellPartType t1 = getCARType(v1);
        CellPartType t2 = getCARType(v2);

        if (t1 != t2) return -1;

        if (getCARType(v1) == PTR_CELL && getCARType(car(v1)) == PTR_CELL)
        {
            int r = eqList(CONTEXT_PTR, car(v1), car(v2));
            if (0 != r) return r;
        }
        else if (getCARType(v1) != PTR_CELL)
        {
            int r = equal(CONTEXT_PTR, v1, v2);
            if (0 != r) return r;
        }
        else
        {
            int r = equal(CONTEXT_PTR, car(v1), car(v2));
            if (0 != r) return r;
        }

        v1 = cdr(v1);
        v2 = cdr(v2);
    }

    if (v1 != Nil) return -1;

    if (v2 != Nil) return 1;

    return 0;
}