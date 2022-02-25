#include <lisp.h>

void makeError(any ex)
{
    err(ex, NULL, "Not making");
}
