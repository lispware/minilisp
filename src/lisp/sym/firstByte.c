#include <lisp.h>

int firstByte(Context*CONTEXT_PTR, any s)
{
    if (getCARType(s) == TXT)
    {
        return ((uword)(s->car)) & 0xff;
    }
    else if (getCARType(s) == BIN_START)
    {
        return ((uword)(s->car->car)) & 0xff;
    }
    else
    {
        giveup("Cant get first byte");
        return -1;
    }
}
