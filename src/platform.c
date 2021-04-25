#include "lisp.h"
#include "cell.h"
#include "platform/platform.h"

#if INTPTR_MAX == INT32_MAX
    #include "def32.d"
#elif INTPTR_MAX == INT64_MAX
    #include "def64.d"
#else
    #error "Unsupported bit width"
#endif


any doSleep(Context *CONTEXT_PTR, any ex)
{
    uword n;
    any x,y;
    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = unBox(y);

    plt_sleep(n);

    return car(x);
}

any doBind(Context *CONTEXT_PTR, any ex)
{
    uword n;
    any x,y;
    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = unBox(y);

    return plt_bind(CONTEXT_PTR, n);
}

any doListen(Context *CONTEXT_PTR, any ex)
{
    uword n;
    any x,y;
    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = unBox(y);

    return plt_listen(CONTEXT_PTR, n);
}

any doSocket(Context *CONTEXT_PTR, any ex)
{
    // uword n;
    // any x,y;
    // x = cdr(ex);
    // if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
    //     return Nil;
    // NeedNum(ex,y);
    // n = unBox(y);

    return plt_socket(CONTEXT_PTR, ex);
}

any doHTTP(Context *CONTEXT_PTR, any ex)
{
    // uword n;
    // any x,y;
    // x = cdr(ex);
    // if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
    //     return Nil;
    // NeedNum(ex,y);
    // n = unBox(y);

    return plt_http(CONTEXT_PTR, ex);
}

any doConnect(Context *CONTEXT_PTR, any ex)
{
    // uword n;
    // any x,y;
    // x = cdr(ex);
    // if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
    //     return Nil;
    // NeedNum(ex,y);
    // n = unBox(y);

    return plt_connect(CONTEXT_PTR, ex);
}

any doSocketClose(Context *CONTEXT_PTR, any ex)
{
    return plt_socket_close(CONTEXT_PTR, ex);
}
