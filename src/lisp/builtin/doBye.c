#include <lisp.h>

// (bye 'num|NIL)
any doBye(Context *CONTEXT_PTR, any ex)
{
   printf("\n");
   bye(0);
   return ex;
}

