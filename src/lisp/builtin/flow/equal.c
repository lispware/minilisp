#include <lisp.h>
#include <tommath.h>

int equal(Context *CONTEXT_PTR, any v, any v2)
{
    CellPartType vt = getCARType(v);
    CellPartType t = getCARType(v2);

    if (t != vt)
    {
        return 1;
    }

    if (t == EXT)
    {
        external *e1 = (external*)v->car;
        external *e2 = (external*)v2->car;
        return e1->equal(CONTEXT_PTR, e1, e2);
    }
    else if (isSym(v2))
    {
        any p1 = car(v);
        any p2 = car(v2);
        do
        {
            if (car(p1) != car(p2))
            {
                return ((uword)car(p2) > (uword)car(p1)) ? -1 : 1;
            }
            p1 = cdr(p1);
            p2 = cdr(p2);
        }
        while (p1 != Nil);

        return p2 == Nil? 0 : 1;
    }
    else if (isCell(v2))
    {
        return eqList(CONTEXT_PTR, v, v2);
    }
    else
    {
        if ( v != v2)
        {
            return 1;
        }
    }

    return 0;
}
