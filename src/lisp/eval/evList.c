#include <lisp.h>

any evList(Context *CONTEXT_PTR, any ex)
{
    any foo;

    if (ex == Nil) return Nil;

    if (isNum(foo = car(ex)))
        return ex;

    if (isCell(foo))
    {
        if (isFunc(foo = evList(CONTEXT_PTR, foo)))
        {
            return evSubr(foo->car,ex);
        }
        return evList2(CONTEXT_PTR, foo,ex);
    }

    for (;;)
    {
        if (isNil(val(foo)))
            undefined(foo,ex);
        if (isNum(foo = val(foo)))
            return foo;
        if (isFunc(foo))
            return evSubr(foo->car,ex);
        if (isCell(foo))
            return evExpr(CONTEXT_PTR, foo, cdr(ex));

    }
}
