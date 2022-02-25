#include <lisp.h>
#include <tommath.h>

int eqList(Context *CONTEXT_PTR, any v1, any v2)
{
    while(!isNil(v1))
    {
        if (isNil(v2)) return 1;

        CellPartType t1 = GetType(v1);
        CellPartType t2 = GetType(v2);

        if (t1 != t2) return -1;

        if (isCell(v1) && isCell(car(v1)))
        {
            int r = eqList(CONTEXT_PTR, car(v1), car(v2));
            if (0 != r) return r;
        }
        else if (!isCell(v1))
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

    if (!isNil(v1)) return -1;

    if (!isNil(v2)) return 1;

    return 0;
}
