#include <lisp.h>

void numError(any ex, any x)
{
    err(ex, x, "Number expected");
}
