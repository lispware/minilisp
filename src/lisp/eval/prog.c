#include <lisp.h>

any prog(Context *CONTEXT_PTR, any x)
{
   any y;

   do
   {
      y = EVAL(CONTEXT_PTR, car(x));
   }
   while (Nil != (x = cdr(x)));

   return y;
}