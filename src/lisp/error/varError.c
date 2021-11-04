#include <lisp.h>

void varError(any ex, any x)
{
    err(ex, x, "Variable expected");
}
