#include <lisp.h>

any isIntern(Context *CONTEXT_PTR, any nm, any tree[2])
{
    any x, y, z;
    word n;

    for (x = tree[1];  x != Nil;)
    {
        y = nm->car;
        z = x->car->car;
        while ((n = (word)(car(y)) - (word)car(z)) == 0)
        {
            if (getCARType(y) != BIN) return car(x);
            y=y->cdr;
            z=z->cdr;
        }

        x = n<0? cadr(x) : cddr(x);
    }



    return NULL;
}
