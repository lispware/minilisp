#include "lisp.h"
#include "cell.h"

#if INTPTR_MAX == INT32_MAX
    #include "def32.d"
#elif INTPTR_MAX == INT64_MAX
    #include "def64.d"
#else
    #error "Unsupported bit width"
#endif


/* Make number from symbol */
any symToNum(Context *CONTEXT_PTR, any sym, int scl, int sep, int ign)
{
    unsigned c;
    int i;
    uword w;
    bool sign, frac;
    word n;
    any s = sym;


    if (!(c = getByte1(&i, &w, &s)))
        return NULL;

    while (c <= ' ')  /* Skip white space */
    {
        if (!(c = getByte(&i, &w, &s)))
            return NULL;
    }

    sign = NO;
    if (c == '+'  ||  c == '-' && (sign = YES))
    {
        if (!(c = getByte(&i, &w, &s)))
            return NULL;
    }

    if ((c -= '0') > 9)
        return NULL;

    frac = NO;
    n = c;
    while ((c = getByte(&i, &w, &s))  &&  (!frac || scl))
    {
        if ((int)c == sep)
        {
            if (frac)
                return NULL;
            frac = YES;
        }
        else if ((int)c != ign)
        {
            if ((c -= '0') > 9)
                return NULL;
            n = n * 10 + c;
            if (frac)
                --scl;
        }
    }
    if (c)
    {
        if ((c -= '0') > 9)
            return NULL;
        if (c >= 5)
            n += 1;
        while (c = getByte(&i, &w, &s))
        {
            if ((c -= '0') > 9)
                return NULL;
        }
    }
    if (frac)
        while (--scl >= 0)
            n *= 10;


    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)n;
    r->type.parts[0] = NUM;

    return r;
}

any mkNum(Context *CONTEXT_PTR, word n)
{
    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)n;
    r->type.parts[0] = NUM;
    return r;
}

// (+ 'num ..) -> num
any doAdd(Context *CONTEXT_PTR, any ex)
{
    any x, y;
    uword n=0;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = unBox(y);
    while (Nil != (x = cdr(x)))
    {
        if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
            return Nil;
        NeedNum(ex,y);
        n += unBox(y);
    }

    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)n;
    r->type.parts[0] = NUM;
    return r;
}

any doSub(Context *CONTEXT_PTR, any ex)
{
    any x, y;
    uword n=0;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = unBox(y);
    while (Nil != (x = cdr(x)))
    {
        if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
            return Nil;
        NeedNum(ex,y);
        n -= unBox(y);
    }

    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)n;
    r->type.parts[0] = NUM;
    return r;
}

any doMul(Context *CONTEXT_PTR, any ex)
{
    any x, y;
    uword n=0;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = unBox(y);
    while (Nil != (x = cdr(x)))
    {
        if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
            return Nil;
        NeedNum(ex,y);
        n *= unBox(y);
    }

    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)n;
    r->type.parts[0] = NUM;
    return r;
}
