#include <lisp.h>

any consName(Context *CONTEXT_PTR, uword w, any n)
{
   cell *p;

   if (!(p = CONTEXT_PTR->Avail))
   {
      gc(CONTEXT_PTR, CELLS);
      p = CONTEXT_PTR->Avail;
   }
   CONTEXT_PTR->Avail = p->car;
   p = symPtr(p);
   p->car = (any)w;
   p->cdr = n;
   setCDRType(p, PTR_CELL);
   return p;
}
