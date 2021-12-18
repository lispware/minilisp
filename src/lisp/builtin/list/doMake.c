#include <lisp.h>

// (make .. [(made 'lst ..)] .. [(link 'any ..)] ..) -> any
any doMake(Context *CONTEXT_PTR, any x)
{
    any *make, *yoke;
    cell c1;

    Push(c1, Nil);
    make = CONTEXT_PTR->Env.make;
    yoke = CONTEXT_PTR->Env.yoke;
    CONTEXT_PTR->Env.make = CONTEXT_PTR->Env.yoke = &data(c1);

    while (!isNil(x = cdr(x)))
    {
        if (isCell(car(x)))
        {
            evList(CONTEXT_PTR, car(x));
        }
    }
    CONTEXT_PTR->Env.yoke = yoke;
    CONTEXT_PTR->Env.make = make;
    return Pop(c1);
}
