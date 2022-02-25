#include <lisp.h>
#include <tommath.h>

void outNum(Context *CONTEXT_PTR, any n)
{
    int len;
    mp_err _mp_error = mp_radix_size((mp_int*)car(n), 10, &len);
    char *buf = (char*)malloc(len);
    _mp_error = mp_to_radix(num(n), buf, len, NULL, 10);
    outString(CONTEXT_PTR, buf);
    free(buf);
}
