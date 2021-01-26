#include "lisp.h"
#include "cell.h"

#if INTPTR_MAX == INT32_MAX
    #include "def32.d"
#elif INTPTR_MAX == INT64_MAX
    #include "def64.d"
#else
    #error "Unsupported bit width"
#endif



any doNot(any x) {
   any a;

   if (isNil(a = EVAL(cadr(x))))
      return T;
   val(At) = a;
   return Nil;
}

// (c...r 'lst) -> any
any doCar(any ex)
{
   any x = cdr(ex);
   x = EVAL(car(x));
   NeedLst(ex,x);
   return car(x);
}

any doCdr(any ex)
{
   any x = cdr(ex);
   x = EVAL(car(x));
   NeedLst(ex,x);
   return cdr(x);
}

any doCons(any x)
{
   any y;
   cell c1;

   x = cdr(x);
   Push(c1, y = cons(EVAL(car(x)),Nil));
   while (Nil != (cdr(x = cdr(x))))
   {
      y = cdr(y) = cons(EVAL(car(x)),Nil);
   }
   cdr(y) = EVAL(car(x));
   return Pop(c1);
}

