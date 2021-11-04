#include <lisp.h>

any evList2(Context *CONTEXT_PTR, any foo, any ex)
{
    cell c1;

    Push(c1, foo);
    if (isCell(foo))
    {
        foo = evExpr(CONTEXT_PTR, foo, cdr(ex));
        drop(c1);
        return foo;
    }

    for (;;)
    {
        if (isNil(val(foo)))
            undefined(foo,ex);

        if (isFunc(foo = val(foo)))
        {
            foo = evSubr(foo->car,ex);
            drop(c1);
            return foo;
        }

        if (isCell(foo))
        {
            foo = evExpr(CONTEXT_PTR, foo, cdr(ex));
            drop(c1);
            return foo;
        }
    }
}
