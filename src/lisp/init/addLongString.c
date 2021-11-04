#include <lisp.h>

any addLongString(any *Mem, any m, char *s)
{
    int ctr = 0;
    int shift = 0;
    setCARType(m, BIN_START);
    setCDRType(m, PTR_CELL);
    m->car = m + 1;
    m++;
    while(*s)
    {
        setCARType(m, BIN);
        setCDRType(m, PTR_CELL);

        ((*(WORD_TYPE*)m))|=(((WORD_TYPE)*s)<<shift) ;
        shift += 8;
        if (++ctr == LISP_WORD_SIZE)
        {
            ctr=0;
            shift = 0;
            if (*(s+1))
            {
                m->cdr = m + 1;
                m++;
            }
        }
        s++;
    }

    m->cdr = *Mem;//TODO

    return m + 1;
}
