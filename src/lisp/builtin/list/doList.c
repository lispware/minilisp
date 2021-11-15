#include <lisp.h>

// (list 'any ['any ..]) -> lst
any doList(Context *CONTEXT_PTR, any x)
{
   any y;
   cell c1;

   x = cdr(x);
   Push(c1, y = cons(CONTEXT_PTR, EVAL(CONTEXT_PTR, car(x)),Nil));
   while (Nil != (x = cdr(x)))
      y = cdr(y) = cons(CONTEXT_PTR, EVAL(CONTEXT_PTR, car(x)),Nil);
   return Pop(c1);
}
