#include <lisp.h>

any EVAL(Context *CONTEXT_PTR, any x)
{
    if (x == T)
    {
        return T;
    }
    else if (getCARType(x) == EXT)
    {
        return x;
    }
    else if (isFunc(x))
    {
        // TODO - we need to fix the FUNC value perhaps
        return x;
    }
    else if (isNum(x))
    {
        return x;
    }
    else if (getCARType(x) == BIN_START)
    {
        return val(x);
    }
    else
    {
        return evList(CONTEXT_PTR, x);
    }
}
