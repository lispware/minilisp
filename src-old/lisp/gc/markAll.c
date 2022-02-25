#include <lisp.h>

void markAll(Context *CONTEXT_PTR)
{
   any p;
   int i;

   for (i = 0; i < MEMS; i ++)
   {
       setMark((any)(CONTEXT_PTR->Mem + i), 0);
       mark(CONTEXT_PTR, (any)(CONTEXT_PTR->Mem + i));
   }

   /* Mark */
   setMark(CONTEXT_PTR->Intern[0], 0);mark(CONTEXT_PTR, CONTEXT_PTR->Intern[0]);
   setMark(CONTEXT_PTR->Intern[1], 0);mark(CONTEXT_PTR, CONTEXT_PTR->Intern[1]);
   setMark(CONTEXT_PTR->Transient[0], 0);mark(CONTEXT_PTR, CONTEXT_PTR->Transient[0]);
   setMark(CONTEXT_PTR->Transient[1], 0);mark(CONTEXT_PTR, CONTEXT_PTR->Transient[1]);
   if (CONTEXT_PTR->ApplyArgs) setMark(CONTEXT_PTR->ApplyArgs, 0);mark(CONTEXT_PTR, CONTEXT_PTR->ApplyArgs);
   if (CONTEXT_PTR->ApplyBody) setMark(CONTEXT_PTR->ApplyBody, 0);mark(CONTEXT_PTR, CONTEXT_PTR->ApplyBody);
   for (p = CONTEXT_PTR->Env.stack; p; p = cdr(p))
   {
      mark(CONTEXT_PTR, car(p));
   }
   for (p = (any)CONTEXT_PTR->Env.bind;  p;  p = (any)((bindFrame*)p)->link)
   {
      for (i = ((bindFrame*)p)->cnt;  --i >= 0;)
      {
         mark(CONTEXT_PTR, ((bindFrame*)p)->bnd[i].sym);
         mark(CONTEXT_PTR, ((bindFrame*)p)->bnd[i].val);
      }
   }
   for (p = (any)CONTEXT_PTR->CatchPtr; p; p = (any)((catchFrame*)p)->link)
   {
      if (((catchFrame*)p)->tag)
         mark(CONTEXT_PTR, ((catchFrame*)p)->tag);
      mark(CONTEXT_PTR, ((catchFrame*)p)->fin);
   }
}
