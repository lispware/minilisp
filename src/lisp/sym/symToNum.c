#include <lisp.h>
#include <tommath.h>

any symToNum(Context *CONTEXT_PTR, any sym, int scl, int sep, int ign)
{
    unsigned c;
    int i;
    uword w;
    bool sign, frac;
    any s = sym;
    int base = 10;



    if (!(c = getByte1(CONTEXT_PTR, &i, &w, &s)))
    {
        return NULL;
    }

    while (c <= ' ')  /* Skip white space */
    {
        if (!(c = getByte(CONTEXT_PTR, &i, &w, &s)))
        {
            return NULL;
        }
    }

    int LEN = pathSize(CONTEXT_PTR, sym);
    int CTR = 0;
    char *str = (char *)calloc(LEN, 1);
    
    sign = NO;
    if (c == '+'  ||  c == '-' && (sign = YES))
    {
        str[CTR++] = c;
        if (!(c = getByte(CONTEXT_PTR, &i, &w, &s)))
        {
            goto returnNULL;
        }
    }
    str[CTR++] = c;

    if ((c -= '0') > 9)
    {
        goto returnNULL;
    }

    if (c == 0)
    {
        c = getByte(CONTEXT_PTR, &i, &w, &s);
        if (c == 'x' || c == 'X') base = 16;
        else str[CTR++] = c;
    }


    while (c = getByte(CONTEXT_PTR, &i, &w, &s))
    {
        if ((int)c != ign)
        {
            str[CTR++] = c;
            if ((c -= '0') > 9)
            {
                goto returnNULL;
            }
        }
    }

    if (c)
    {
        if (c == 'H' || c == 'h') base = 16;
        else if (c == 'B' || c == 'b') base = 2;
        else if ((c -= '0') > 9) goto returnNULL;
    }

    mp_int *BIGNUM = (mp_int*)malloc(sizeof(mp_int));
    mp_err _mp_error = mp_init(BIGNUM); // TODO handle the error appropriately
    _mp_error = mp_read_radix(BIGNUM, str, base);
    free(str);

    NewNumber(ext, BIGNUM, r);
    return r;

returnNULL:
    free(str);
    return NULL;

}
