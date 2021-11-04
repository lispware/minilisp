#include <lisp.h>
#include <tommath.h>
#include "net.h"

any doBind(Context *CONTEXT_PTR, any ex)
{
    uword n;
    any x,y;
    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = mp_get_i32((mp_int*)y->car);

    return pltBind(CONTEXT_PTR, n);
}
