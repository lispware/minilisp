#include <lisp.h>

void atomError(any ex, any x)
{
    err(ex, x, "Atom expected");
}
