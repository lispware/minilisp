#include <lisp.h>

// (args) -> flg
any doArgs(Context *CONTEXT_PTR, any ex)
{
   return CONTEXT_PTR->Env.next > 0? T : Nil;
}
