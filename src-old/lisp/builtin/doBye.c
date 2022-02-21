#include <lisp.h>

// (bye 'num|NIL)
any doBye(Context *CONTEXT_PTR, any ex)
{
   bye(0);
   return ex;
}

