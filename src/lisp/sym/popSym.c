#include <lisp.h>

any popSym(Context *CONTEXT_PTR, int i, uword n, any q, cell *cp)
{
    if (q)
    {
        //val(q) = i <= (BITS-2)? box(n) : consName(CONTEXT_PTR, n, Zero);
        q->cdr = consName(CONTEXT_PTR, n, Nil);
        return Pop(*cp);
    }
    return consSym(CONTEXT_PTR, NULL, n);
}
