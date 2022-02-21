#include <lisp.h>

// (++ var) -> any
any doPopq(Context *CONTEXT_PTR, any ex)
{
    any p1 = cadr(ex);

    if (!isSym(p1))
    {
        return p1;
    }

    any theList = cdr(p1);

    any r = cdr(theList);

    cdr(p1) = r;

    return car(theList);
}
