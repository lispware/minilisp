#include <lisp.h>
#include <tommath.h>

any doDo(Context *CONTEXT_PTR, any x)
{
    mp_err _mp_error;
    any f, y, z, a;
    mp_int CTR, ONE;
    _mp_error = mp_init(&ONE);
    _mp_error = mp_init(&CTR);

    mp_set(&ONE, 1); // TODO - Free the ints

    x = cdr(x);
    if (isNil(f = EVAL(CONTEXT_PTR, car(x))))
    {
        mp_clear_multi(&ONE, &CTR, NULL);
        return Nil;
    }
    else
    {
        _mp_error = mp_copy(num(f), &CTR);
    }

    x = cdr(x),  z = Nil;
    for (;;)
    {
        int cmp = mp_cmp(&CTR, &ONE); 
        if (cmp >= 0)
        {
            _mp_error = mp_decr(&CTR);
        }
        else
        {
            mp_clear_multi(&ONE, &CTR, NULL);
            return z;
        }
        y = x;
        do
        {
            if (!isNum(z = car(y)))
            {
                if (isNil(car(z)))
                {
                    z = cdr(z);
                    if (isNil(a = EVAL(CONTEXT_PTR, car(z))))
                    {
                        mp_clear_multi(&ONE, &CTR, NULL);
                        return prog(CONTEXT_PTR, cdr(z));
                    }
                    val(At) = a;
                    z = Nil;
                }
                else if (car(z) == T)
                {
                    z = cdr(z);
                    if (!isNil(a = EVAL(CONTEXT_PTR, car(z))))
                    {
                        val(At) = a;
                        mp_clear_multi(&ONE, &CTR, NULL);
                        return prog(CONTEXT_PTR, cdr(z));
                    }
                    z = Nil;
                }
                else
                {
                    z = evList(CONTEXT_PTR, z);
                }
            }
        } while (!isNil(y = cdr(y)));
    }
}
