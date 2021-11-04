#include <lisp.h>
#include <tommath.h>

any doCall(Context *CONTEXT_PTR, any ex)
{
    any y;
    any x = cdr(ex);

    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
    {
        return Nil;
    }

    CellPartType t = getCARType(y);
    if (t != TXT && t != BIN_START)
    {
        return Nil;
    }

    int len = pathSize(CONTEXT_PTR, y);
    char *buf = (char *)calloc(len, 1);
    sym2str(CONTEXT_PTR, y, buf);
    int ret = system(buf);
    free(buf);

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    mp_err _mp_error = mp_init(n); // TODO handle the errors appropriately
    mp_set_i32(n, ret);
    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)n;
    r->meta.type.parts[0] = NUM;

    return r;
}
