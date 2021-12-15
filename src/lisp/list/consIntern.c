#include <lisp.h>

any consIntern(Context *CONTEXT_PTR, any x, any y)
{
    any r = cons(CONTEXT_PTR, x, y);

    setCARType(r, PTR_CELL);

    return r;
}
