#include <lisp.h>

void *allignedAlloc(size_t size)
{
    char *p = (char*)calloc(size + 8 + sizeof(void*), 1);
    char *q = p;
    for(int i = 0;i < 8; i++)
    {
        uword w = (uword)p;
        if (!(w & 0x7))
        {
            q = p;
        }
        if (w & 0x7) p++;
    }

    return q;
}
