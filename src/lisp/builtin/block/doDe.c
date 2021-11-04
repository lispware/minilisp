#include <lisp.h>

// (de sym . any) -> sym
any doDe(Context *CONTEXT_PTR, any ex)
{
    any s = cadr(ex);
    val(s) = cddr(ex);
    return s;
}

