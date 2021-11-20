#include <lisp.h>

uword length(Context *CONTEXT_PTR, any x)
{
   uword n;

   if (getCARType(x) != PTR_CELL) return 0;
   if (cdr(x) == x) return 0;

   for (n = 0; x != Nil; x = cdr(x)) ++n;
   return n;
}
