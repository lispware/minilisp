#include <lisp.h>


any doRun(Context *CONTEXT_PTR, any x)
{
   any y;
   cell c1;
   bindFrame *p;

   x = cdr(x),  data(c1) = EVAL(CONTEXT_PTR, car(x)),  x = cdr(x);
   if (!isNum(data(c1)))
   {
      Save(c1);
      if (!isNum(y = EVAL(CONTEXT_PTR, car(x))) || !(p = CONTEXT_PTR->Env.bind))
         data(c1) = run(CONTEXT_PTR, data(c1));
   }
   return Pop(c1);
}
