#include <lisp.h>

void initialize_context(Context *CONTEXT_PTR)
{
   heapAlloc(CONTEXT_PTR);
   CONTEXT_PTR->Intern[0] = CONTEXT_PTR->Intern[1] = CONTEXT_PTR->Transient[0] = CONTEXT_PTR->Transient[1] = Nil;

   for (int i = 1; i < MEMS; i++)
   {
      any cell = (any)(CONTEXT_PTR->Mem + i);
      CellPartType carType = getCARType(cell);
      CellPartType cdrType = getCDRType(cell);

      if ((BIN_START == carType || TXT == carType))
      {
         intern(CONTEXT_PTR, cell, CONTEXT_PTR->Intern);
      }
   }
}