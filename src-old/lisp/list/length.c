#include <lisp.h>

uword length(Context *CONTEXT_PTR, any x)
{
   uword n;

   if (!isCell(x)) return 0;
   if (cdr(x) == x) return 0;

   for (n = 0; !isNil(x); x = cdr(x)) ++n;
   return n;
}
