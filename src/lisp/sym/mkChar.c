#include <lisp.h>

any mkChar(Context *CONTEXT_PTR, int c)
{
   return consSym(CONTEXT_PTR, NULL, c);
}
