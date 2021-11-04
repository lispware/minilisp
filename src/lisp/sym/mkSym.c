#include <lisp.h>

any mkSym(Context *CONTEXT_PTR, byte *s)
{
    int i;
    uword w;
    cell c1, *p;

    putByte1(*s++, &i, &w, &p);
    while (*s)
    {
        putByte(CONTEXT_PTR, *s++, &i, &w, &p, &c1);
    }
    return popSym(CONTEXT_PTR, i, w, p, &c1);
}
