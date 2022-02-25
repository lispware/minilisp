#include <lisp.h>

any consIntern(Context *CONTEXT_PTR, any x, any y)
{
    dump("consIntern1");
    any r = cons(CONTEXT_PTR, x, y);
    dump("consIntern2");

    setCARType(r, PTR_CELL);
    dump("consIntern3");

    return r;
}
