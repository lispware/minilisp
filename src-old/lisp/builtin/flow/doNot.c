#include <lisp.h>

any doNot(Context *CONTEXT_PTR, any x)
{
   any a;

   if (isNil(a = EVAL(CONTEXT_PTR, cadr(x))))
      return T;
   val(At) = a;
   return Nil;
}
