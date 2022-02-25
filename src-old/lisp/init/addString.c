#include <lisp.h>

any addString(any *Mem, any m, char *s)
{
    int ctr = 0;
    int shift = 0;
    setCARType(m, BIN_START);
    car(m) = m + 1;
    m++;
    while(*s)
    {
        setCARType(m, BIN);

        ((*(WORD_TYPE*)m))|=(((WORD_TYPE)*s)<<shift) ;
        shift += 8;
        if (++ctr == LISP_WORD_SIZE)
        {
            ctr=0;
            shift = 0;
            if (*(s+1))
            {
                cdr(m) = m + 1;
                setCARType(m, BIN);
                m++;
            }
        }
        s++;
    }

    cdr(m) = *Mem;//TODO
    setCARType(m, BIN);

    return m + 1;
}
