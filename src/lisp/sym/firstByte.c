#include <lisp.h>

int firstByte(Context*CONTEXT_PTR, any s)
{
    if (isSym(s))
    {
        return ((uword)(car(car(s)))) & 0xff;
    }
    else
    {
        giveup("Cant get first byte");
        return -1;
    }
}
