#include <lisp.h>

// (line 'flg) -> lst|sym
any doLine(Context *CONTEXT_PTR, any x)
{
   any y;
   int i;
   uword w;
   cell c1;

   if (!CONTEXT_PTR->Chr)
      CONTEXT_PTR->Env.get(CONTEXT_PTR);
   if (eol(CONTEXT_PTR))
      return Nil;
   x = cdr(x);
   if (isNil(EVAL(CONTEXT_PTR, car(x)))) {
      Push(c1, cons(CONTEXT_PTR, mkChar(CONTEXT_PTR, CONTEXT_PTR->Chr), Nil));
      y = data(c1);
      for (;;) {
         if (CONTEXT_PTR->Env.get(CONTEXT_PTR), eol(CONTEXT_PTR))
            return Pop(c1);
         any c = mkChar(CONTEXT_PTR, CONTEXT_PTR->Chr);
         cdr(y) = cons(CONTEXT_PTR, c, Nil);
         setCARType(y, PTR_CELL);
         y = cdr(y);
      }
   }
   else {
      putByte1(CONTEXT_PTR->Chr, &i, &w, &y);
      for (;;) {
         if (CONTEXT_PTR->Env.get(CONTEXT_PTR), eol(CONTEXT_PTR))
            return popSym(CONTEXT_PTR, i, w, y, &c1);
         putByte(CONTEXT_PTR, CONTEXT_PTR->Chr, &i, &w, &y, &c1);
      }
   }
}
