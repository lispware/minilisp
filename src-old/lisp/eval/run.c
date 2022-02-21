#include <lisp.h>

any run(Context *CONTEXT_PTR, any x)
{
   any y;
   cell at;

   Push(at,val(At));
   do
   {
      y = EVAL(CONTEXT_PTR, car(x));
   }
   while (!isNil(x = cdr(x)));
   val(At) = Pop(at);
   return y;
}
