#include <lisp.h>

void newline(Context *CONTEXT_PTR)
{
    CONTEXT_PTR->Env.put(CONTEXT_PTR, '\n');
}
