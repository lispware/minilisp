#include <lisp.h>

// (or 'any ..) -> any
any doOr(Context *CONTEXT_PTR, any x) {
   any a;

   x = cdr(x);
   do
   {
      if (!isNil(a = EVAL(CONTEXT_PTR, car(x))))
      {
         return val(At) = a;
      }
   } while (!isNil(x = cdr(x)));

   return Nil;
}
