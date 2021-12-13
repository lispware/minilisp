#include <lisp.h>
#include <tommath.h>

any doSwitchBase(Context *CONTEXT_PTR, any ex)
{
    any p1 = cadr(ex);
    any p2 = caddr(ex);
    int base = 10;

    p1 = EVAL(CONTEXT_PTR, p1);
    CellPartType t = getCARType(p1);

    p2 = EVAL(CONTEXT_PTR, p2);
    if (isNum(p2))
    {
        base = mp_get_i32((mp_int*)p2->car);
    }

    if (isNum(p1))
    {
        int len;
        mp_err _mp_error = mp_radix_size(num(p1), base, &len);
        char *buf = (char*)malloc(len);
        _mp_error = mp_to_radix(num(p1), buf, len, NULL, base);
        any r = mkSym(CONTEXT_PTR, (byte*)buf);
        free(buf);
        return r;
    }
    else if (isSym(p1))
    {
        int LEN = pathSize(CONTEXT_PTR, p1);
        int CTR = 0;
        char *str = (char *)calloc(LEN, 1);
        sym2str(CONTEXT_PTR, p1, str);

        mp_int *BIGNUM = (mp_int*)malloc(sizeof(mp_int));
        mp_err _mp_error = mp_init(BIGNUM); // TODO handle the error appropriately
        _mp_error = mp_read_radix(BIGNUM, str, base);
        free(str);

        NewExtNum(ext, BIGNUM);

        any r = cons(CONTEXT_PTR, Nil, Nil);
        r->car = (any)ext;
        r->meta.type.parts[0] = EXT_NUM;

        return r;
    }

    return Nil;
}

