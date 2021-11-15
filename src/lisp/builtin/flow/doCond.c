#include <lisp.h>

// (if 'any1 any2 . prg) -> any
any doCond(Context *CONTEXT_PTR, any x)
{
   any a;

   while (Nil != (x = cdr(x)))
   {
      if (!isNil(a = EVAL(CONTEXT_PTR, caar(x))))
      {
         val(At) = a;
         return prog(CONTEXT_PTR, cdar(x));
      }
   }
   return Nil;
}
