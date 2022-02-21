#include <lisp.h>

// (quote . any) -> any
any doQuote(Context *CONTEXT_PTR, any x)
{
    return cdr(x);
}

