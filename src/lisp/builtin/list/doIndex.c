#include <lisp.h>
#include <tommath.h>

// (index 'any 'lst) -> cnt | NIL
any doIndex(Context *CONTEXT_PTR, any x)
{
   any n;
   cell c1;

   x = cdr(x);
   Push(c1, EVAL(CONTEXT_PTR, car(x)));

   x = cdr(x);
   x = EVAL(CONTEXT_PTR, car(x));

   if (isNil(x)) return Nil;
   if (!isCell(x)) return Nil;

   return indx(CONTEXT_PTR, Pop(c1), x);
}
