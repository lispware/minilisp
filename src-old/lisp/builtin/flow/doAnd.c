#include <lisp.h>

// (and 'any ..) -> any
any doAnd(Context *CONTEXT_PTR, any x) {
   any a;

   x = cdr(x);
   do
   {
      if (isNil(a = EVAL(CONTEXT_PTR, car(x))))
         return Nil;
      val(At) = a;
   } while (!isNil(x = cdr(x)));
   return a;
}
