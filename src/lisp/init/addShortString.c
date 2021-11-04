#include <lisp.h>

any addShortString(any m, char *s)
{
    int ctr = 0;
    int shift = 0;
    setCARType(m, TXT);
    setCDRType(m, PTR_CELL);
    for (int i = 0; *s && i < LISP_WORD_SIZE; i++)
    {
        ((*(WORD_TYPE*)m))|=(((WORD_TYPE)*s)<<shift) ;
        shift += 8;
        s++;
    }

    return m + 1;
}
