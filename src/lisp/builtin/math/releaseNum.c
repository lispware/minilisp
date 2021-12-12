#include <lisp.h>
#include <tommath.h>

void releaseNum(external *p)
{
    if (p->type != EXT_NUM)
    {
        fprintf(stderr, "Not a number %d\n", p->type);
        exit(0);
    }

    mp_clear((mp_int*)p->pointer);
    free(p);
}
