#include <lisp.h>
#include <stdio.h>
#include <stdlib.h>
#include <tommath.h>

char * printExtNum(Context *CONTEXT_PTR, struct _external* obj)
{
    int len;
    mp_err _mp_error = mp_radix_size((mp_int*)obj->pointer, 10, &len);
    char *buf = (char*)malloc(len);
    _mp_error = mp_to_radix((mp_int*)obj->pointer, buf, len, NULL, 10);
    return buf;
}
