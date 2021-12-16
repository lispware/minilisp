#include <lisp.h>

any consName(Context *CONTEXT_PTR, uword w, any n)
{
   cell *p;

   if (!(p = CONTEXT_PTR->Avail))
   {
      gc(CONTEXT_PTR, CELLS);
      p = CONTEXT_PTR->Avail;
   }
   CONTEXT_PTR->Avail = car(p);
   p = symPtr(p);
   car(p) = (any)w;
   cdr(p) = n;
   return p;
}
