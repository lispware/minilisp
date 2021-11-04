#include <lisp.h>

void eofErr(void)
{
    err(NULL, NULL, "EOF Overrun");
}
