#include <lisp.h>

int pathSize(Context *CONTEXT_PTR, any x)
{
    int c = firstByte(CONTEXT_PTR, x);

    if (c != '@'  &&  (c != '+'))
    {
        return bufSize(CONTEXT_PTR, x);
    }

    return symBytes(CONTEXT_PTR, x);
}
