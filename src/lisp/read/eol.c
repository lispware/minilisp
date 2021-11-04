#include <lisp.h>

bool eol(Context *CONTEXT_PTR)
{
   if (CONTEXT_PTR->Chr < 0)
   {
      return YES;
   }

   if (CONTEXT_PTR->Chr == '\n')
   {
      CONTEXT_PTR->Chr = 0;
      return YES;
   }

   if (CONTEXT_PTR->Chr == '\r')
   {
      CONTEXT_PTR->Env.get(CONTEXT_PTR);
      if (CONTEXT_PTR->Chr == '\n')
      {
         CONTEXT_PTR->Chr = 0;
      }
      return YES;
   }

   return NO;
}
