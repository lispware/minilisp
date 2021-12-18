#include <lisp.h>

// (prin 'any ..) -> any
any doPrin(Context *CONTEXT_PTR, any x)
{
   any y = Nil;

   while (!isNil(x = cdr(x)) )
   {
      prin(CONTEXT_PTR, y = EVAL(CONTEXT_PTR, car(x)));
   }
   newline(CONTEXT_PTR);
   return y;
}
