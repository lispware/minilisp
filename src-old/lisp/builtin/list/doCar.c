#include <lisp.h>

// (c...r 'lst) -> any
any doCar(Context *CONTEXT_PTR, any ex)
{
   any x = cdr(ex);
   x = EVAL(CONTEXT_PTR, car(x));
   NeedLst(ex,x);
   return car(x);
}
