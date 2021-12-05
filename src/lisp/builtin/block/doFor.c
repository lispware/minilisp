#include <lisp.h>
#include <tommath.h>

// (for sym 'num ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
// (for sym|(sym2 . sym) 'lst ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
// (for (sym|(sym2 . sym) 'any1 'any2 [. prg]) ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
any doFor(Context *CONTEXT_PTR, any x)
{
    mp_err _mp_error;
    any y, body, cond, a;
    cell c1;
    // struct {  // bindFrame
    //    struct bindFrame *link;
    //    int i, cnt;
    //    struct {any sym; any val;} bnd[2];
    // } f;

    bindFrame *f = allocFrame(2);

    f->link = CONTEXT_PTR->Env.bind,  CONTEXT_PTR->Env.bind = f;
    f->i = 0;

    y = car(x = cdr(x));
    if (!isCell(y) || !isCell(cdr(y)))
    {
        if (!isCell(y))
        {
            f->cnt = 1;
            f->bnd[0].sym = y;
            f->bnd[0].val = val(y);
        }
        else
        {
            f->cnt = 2;
            f->bnd[0].sym = cdr(y);
            f->bnd[0].val = val(cdr(y));
            f->bnd[1].sym = car(y);
            f->bnd[1].val = val(car(y));
            val(f->bnd[1].sym) = Nil;
        }

        y = Nil;
        x = cdr(x),  Push(c1, EVAL(CONTEXT_PTR, car(x)));

        if (isNum(data(c1)))
        {
            f->bnd[0].sym->cdr  = mkNum(CONTEXT_PTR, 0);
        }

        body = x = cdr(x);
        for (;;)
        {
            if (isNum(data(c1)))
            {
                if (! mp_cmp((mp_int*)f->bnd[0].sym->cdr->car, (mp_int*)num(data(c1)->car)))
                    break;

                mp_int *n = (mp_int*)malloc(sizeof(mp_int));
                _mp_error = mp_init(n); // TODO handle the errors appropriately

                _mp_error = mp_copy((mp_int*)f->bnd[0].sym->cdr->car, n);
                _mp_error = mp_incr(n);

                any r = cons(CONTEXT_PTR, Nil, Nil);
                r->car = (any)n;
                r->meta.type.parts[0] = NUM;

                f->bnd[0].sym->cdr  = r;
            }
            else
            {
                if (Nil == (data(c1)))
                {
                    break;
                }
                val(f->bnd[0].sym) = car(data(c1));
                if (Nil == (data(c1) = cdr(data(c1))))
                {
                    data(c1) = Nil;
                }
            }
            do
            {
                if (!isNum(y = car(x)))
                {
                    if (isNil(car(y)))
                    {
                        y = cdr(y);
                        if (isNil(a = EVAL(CONTEXT_PTR, car(y))))
                        {
                            y = prog(CONTEXT_PTR, cdr(y));
                            goto for1;
                        }
                        val(At) = a;
                        y = Nil;
                    }
                    else if (car(y) == T)
                    {
                        y = cdr(y);
                        if (!isNil(a = EVAL(CONTEXT_PTR, car(y))))
                        {
                            val(At) = a;
                            y = prog(CONTEXT_PTR, cdr(y));
                            goto for1;
                        }
                        y = Nil;
                    }
                    else
                    {
                        y = evList(CONTEXT_PTR, y);
                    }
                }
            } while (Nil != (x = cdr(x)));
            x = body;
        }
for1:
        drop(c1);
        val(f->bnd[0].sym) = f->bnd[0].val;
        CONTEXT_PTR->Env.bind = f->link;
        free(f);
        return y;
    }

    return Nil;
}
