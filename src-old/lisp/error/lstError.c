#include <lisp.h>

void lstError(any ex, any x)
{
    err(ex, x, "List expected");
}
