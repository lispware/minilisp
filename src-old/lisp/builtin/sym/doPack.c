#include <lisp.h>

// (pack 'any ..) -> sym
any doPack(Context *CONTEXT_PTR, any x)
{
   int i;
   uword w;
   any y;
   cell c1, c2;

   x = cdr(x),  Push(c1, EVAL(CONTEXT_PTR, car(x)));
   putByte0(&i, &w, &y);
   pack(CONTEXT_PTR, data(c1), &i, &w, &y, &c2);
   while (!isNil(x = cdr(x)))
   {
      pack(CONTEXT_PTR, data(c1) = EVAL(CONTEXT_PTR, car(x)), &i, &w, &y, &c2);
   }
   y = popSym(CONTEXT_PTR, i, w, y, &c2);
   drop(c1);

   return i? y : Nil;
}
