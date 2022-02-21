#include <tommath.h>
#include "thread.h"

any doSleep(Context *CONTEXT_PTR, any ex)
{
    uword n;
    any x,y;
    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = mp_get_i32(num(y));

    plt_sleep(n);

    return y;
}
