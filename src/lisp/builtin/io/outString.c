#include <lisp.h>

void outString(Context *CONTEXT_PTR, char *s)
{
    while (*s)
        CONTEXT_PTR->Env.put(CONTEXT_PTR, *s++);
}
