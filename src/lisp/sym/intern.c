#include <lisp.h>

any intern(Context *CONTEXT_PTR, any sym, any tree[2])
{
   any nm, x;
   word n;


   if (getCARType(sym) == BIN_START) return internBin(CONTEXT_PTR, sym, tree);

   nm = sym;

   x = tree[0];
   if (Nil == x)
   {
      tree[0] = consIntern(CONTEXT_PTR, sym, Nil);
      return tree[0];
   }
   for (;;)
   {
      if ((n = (word)(car(nm)) - (word)caar(x)) == 0)
      {
         return car(x);
      }

      if (Nil == cdr(x))
      {
         cdr(x) = n < 0 ? consIntern(CONTEXT_PTR, consIntern(CONTEXT_PTR, sym, Nil), Nil) : consIntern(CONTEXT_PTR, Nil, consIntern(CONTEXT_PTR, sym, Nil));
         return sym;
      }
      if (n < 0)
      {
         if (Nil != cadr(x))
         {
            x = cadr(x);
         }
         else
         {
            cadr(x) = consIntern(CONTEXT_PTR, sym, Nil);
            return sym;
         }
      }
      else
      {
         if (Nil != cddr(x))
         {
            x = cddr(x);
         }
         else
         {
            cddr(x) = consIntern(CONTEXT_PTR, sym, Nil);
            return sym;
         }
      }
   }
}
