#include <lisp.h>
#include <tommath.h>

// (cmp 'any ..) -> flg
any doCmp(Context *CONTEXT_PTR, any x)
{
    mp_err _mp_error;
    cell c1;

    x = cdr(x),  Push(c1, EVAL(CONTEXT_PTR, car(x)));
    x = cdr(x);
    while (x != Nil)
    {
        int r = equal(CONTEXT_PTR, data(c1), EVAL(CONTEXT_PTR, car(x)));
        if (0 != r)
        {
            drop(c1);

            mp_int *id = (mp_int*)malloc(sizeof(mp_int));
            _mp_error = mp_init(id); // TODO handle the errors appropriately
            mp_set_i32(id, r);
            any idr = cons(CONTEXT_PTR, Nil, Nil);
            idr->car = (any)id;
            idr->meta.type.parts[0] = NUM;
            return idr;
        }

        x = cdr(x);
    }

    drop(c1);

    mp_int *id = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(id); // TODO handle the errors appropriately
    mp_set_i32(id, 0);
    any idr = cons(CONTEXT_PTR, Nil, Nil);
    idr->car = (any)id;
    idr->meta.type.parts[0] = NUM;
    return idr;
}
