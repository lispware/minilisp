#include <lisp.h>

any consName(Context *CONTEXT_PTR, uword w, any n)
{
   cell *p;

   if (!(p = CONTEXT_PTR->Avail))
   {
       cell c1;
       Push(c1, n);
      gc(CONTEXT_PTR, CELLS);
      drop(c1);
      p = CONTEXT_PTR->Avail;
   }
   CONTEXT_PTR->Avail = car(p);
   p = symPtr(p);
   car(p) = (any)w;
   cdr(p) = n;
   setCARType(p, PTR_CELL);
   return p;
}
