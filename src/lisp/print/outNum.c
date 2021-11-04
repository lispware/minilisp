#include <lisp.h>
#include <tommath.h>

void outNum(Context *CONTEXT_PTR, any n)
{
    int len;
    mp_err _mp_error = mp_radix_size((mp_int*)n->car, 10, &len);
    char *buf = (char*)malloc(len);
    _mp_error = mp_to_radix((mp_int*)n->car, buf, len, NULL, 10);
    //char *buf = mpz_get_str(NULL,10, (MP_INT*)n->car);
    outString(CONTEXT_PTR, buf);
    free(buf);
}
