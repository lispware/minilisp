#include <lisp.h>

void initialize_context(Context *CONTEXT_PTR)
{
   heapAlloc(CONTEXT_PTR);
   CONTEXT_PTR->Intern[0] = CONTEXT_PTR->Intern[1] = CONTEXT_PTR->Transient[0] = CONTEXT_PTR->Transient[1] = Nil;

   for (int i = 1; i < MEMS; i++)
   {
      any cell = (any)(CONTEXT_PTR->Mem + i);

      if (isSym(cell))
      {
         intern(CONTEXT_PTR, cell, CONTEXT_PTR->Intern);
         dump("symbol");
      }
   }
}
