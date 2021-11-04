#include <lisp.h>

// (next) -> any
any doNext(Context *CONTEXT_PTR, any ex)
{
   if (CONTEXT_PTR->Env.next > 0)
      return data(CONTEXT_PTR->Env.arg[--CONTEXT_PTR->Env.next]);
   if (CONTEXT_PTR->Env.next == 0)
      CONTEXT_PTR->Env.next = -1;
   return Nil;
}
