#include <lisp.h>

int symBytes(Context *CONTEXT_PTR, any x)
{
    int cnt = 0;
    uword w;

    if (isNil(x))
        return 0;

    if (isSym(x))
    {

        x = car(x);
        while (x != Nil)
        {
			w = (uword)(car(x));
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
