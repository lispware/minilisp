#include <lisp.h>

void mark(Context *CONTEXT_PTR, any x)
{
    if (!x) return;

    if (getMark(x)) return;

    setMark(x, 1);

    if (x == Nil) return;

    if (isSym(x))
    {
        mark(CONTEXT_PTR, cdr(x));

        x = x->car;
        while(x && x != Nil)
        {
            mark(CONTEXT_PTR, x);
            x=x->cdr;
        }
        return;
    }

    if (getCARType(x) == PTR_CELL) mark(CONTEXT_PTR, car(x));

    while (1)
    {
        x = cdr(x);
        if (!x) break;
        if (x==Nil) break;
        if (getMark(x)) break;
        setMark(x, 1);
        if (isSym(x))
        {
            setMark(x, 0);
            mark(CONTEXT_PTR, x);
        }
        if (getCARType(x) == PTR_CELL) mark(CONTEXT_PTR, car(x));
    }
}
