#include <lisp.h>

void mark(Context *CONTEXT_PTR, any x)
{
    if (!x) return;

    if (getMark(x)) return;

    setMark(x, 1);

    if (isNil(x)) return;

    if (isSym(x))
    {
        mark(CONTEXT_PTR, cdr(x));

        x = car(x);
        while(x && !isNil(x))
        {
            mark(CONTEXT_PTR, x);
            x=cdr(x);
        }
        return;
    }

    if (isCell(x)) mark(CONTEXT_PTR, car(x));

    while (1)
    {
        x = cdr(x);
        if (!x) break;
        if (isNil(x)) break;
        if (getMark(x)) break;
        setMark(x, 1);
        if (isSym(x))
        {
            setMark(x, 0);
            mark(CONTEXT_PTR, x);
        }
        if (isCell(x)) mark(CONTEXT_PTR, car(x));
    }
}
