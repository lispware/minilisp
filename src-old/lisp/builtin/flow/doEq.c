#include <lisp.h>
#include <tommath.h>


// (= 'any ..) -> flg
any doEq(Context *CONTEXT_PTR, any x)
{
    cell c1;

    x = cdr(x),  Push(c1, EVAL(CONTEXT_PTR, car(x)));
    x = cdr(x);
    while (!isNil(x))
    {
        if (0 != equal(CONTEXT_PTR, data(c1), EVAL(CONTEXT_PTR, car(x))))
        {
            drop(c1);
            return Nil;
        }

        x = cdr(x);
    }

    drop(c1);
    return T;
}
