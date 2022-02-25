#include <lisp.h>

void undefined(any x, any ex)
{
    err(ex, x, "Undefined");
}
