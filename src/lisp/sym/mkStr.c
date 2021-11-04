#include <lisp.h>

any mkStr(Context *CONTEXT_PTR, char *s)
{
   if (s && *s)
   {
      return mkSym(CONTEXT_PTR, (byte *)s);
   }
   else
   {
      return Nil;
   }
}
