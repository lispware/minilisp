#include <lisp.h>

int symBytes(Context *CONTEXT_PTR, any x)
{
    int cnt = 0;
    uword w;

    if (isNil(x))
        return 0;

    CellPartType t = getCARType(x);

    if (t == TXT)
    {
        w = (uword)(x->car);
        while (w)
        {
            ++cnt;
            w >>= 8;
        }
    }
    else if (t == BIN_START)
    {

        x = x->car;
        while (x != Nil)
        {
			w = (uword)(x->car);
            while (w)
            {
                ++cnt;
                w >>= 8;
            }
            x = x->cdr;
        }
    }

    return cnt;
}
