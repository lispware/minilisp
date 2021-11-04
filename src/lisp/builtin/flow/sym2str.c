#include <lisp.h>

void sym2str(Context *CONTEXT_PTR, any nm, char *buf)
{
    int i, c, ctr=0;
    uword w;

    c = getByte1(CONTEXT_PTR, &i, &w, &nm);
    do
    {
        if (c == '"'  ||  c == '\\')
        {
            buf[ctr++]=c;
        }
        buf[ctr++]=c;
    }
    while (c = getByte(CONTEXT_PTR, &i, &w, &nm));
    buf[ctr++]=0;
}
