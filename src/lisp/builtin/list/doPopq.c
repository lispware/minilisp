#include <lisp.h>

// (++ var) -> any
any doPopq(Context *CONTEXT_PTR, any ex)
{
    any p1 = cadr(ex);
    CellPartType t = getCARType(p1);

    if (t != BIN_START)
    {
        return p1;
    }

    any theList = cdr(p1);

    any r = cdr(theList);

    cdr(p1) = r;

    return car(theList);
}
