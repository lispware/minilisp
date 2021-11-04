#include <lisp.h>

any addString(any *Mem, any m, char *s)
{
    int l = strlen(s);
    if (l > LISP_WORD_SIZE)
    {
        return addLongString(Mem, m, s);
    }
    else
    {
        return addShortString(m, s);
    }
}
