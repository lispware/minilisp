#include <lisp.h>
#include "net.h"

any doListen(Context *CONTEXT_PTR, any ex)
{
    uword n;
    any x,y;
    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;

    external *e = (external*)y->car;
    n = (uword)e->pointer;

    return pltListen(CONTEXT_PTR, n);
}
