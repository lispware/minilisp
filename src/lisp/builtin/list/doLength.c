#include <lisp.h>
#include <tommath.h>

// (length 'any) -> cnt | T
any doLength(Context *CONTEXT_PTR, any x)
{
    uword w;
    int n, c;
    any y;
    mp_err _mp_error;
    int lengthBiggerThanZero=0;

    x = EVAL(CONTEXT_PTR, cadr(x));
    CellPartType t = getCARType(x);
    mp_int *r = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(r); // TODO handle the errors appropriately
    mp_set_i32(r, 0);

    if (x == Nil)
    {
        return Nil;
    }
    if (t == NUM)
    {
        return x;
    }
    else if (t == TXT)
    {
        w = (uword)(x->car);
        if (w) lengthBiggerThanZero = 1;
        while (w)
        {
            _mp_error = mp_incr(r);
            w >>= 8;
        }
    }
    else if (t == BIN_START)
    {
        x = x->car;
        while (x != Nil)
        {
            w = (uword)(x->car);
            if (w) lengthBiggerThanZero = 1;
            while (w)
            {
                _mp_error = mp_incr(r);
                w >>= 8;
            }
            x = x->cdr;
        }
    }
    else if (t == PTR_CELL)
    {
        lengthBiggerThanZero = 1;
        while (Nil != x)
        {
            _mp_error = mp_incr(r);
            x = cdr(x);
        }
    }
    else
    {
        return Nil;
    }

    if (!lengthBiggerThanZero) return Nil;

    any l = cons(CONTEXT_PTR, Nil, Nil);
    l->car = (any)r;
    l->meta.type.parts[0] = NUM;
    return l;
}