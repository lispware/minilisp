#include <lisp.h>

int pathSize(Context *CONTEXT_PTR, any x)
{
    return symBytes(CONTEXT_PTR, x) + 1;
}
