#include <lisp.h>

any isIntern(Context *CONTEXT_PTR, any nm, any tree[2])
{
    any x, y, z;
    word n;

    for (x = tree[1]; !isNil(x);)
    {
        y = car(nm);
        z = car(car(x));
        while ((n = (word)(car(y)) - (word)car(z)) == 0)
        {
            if (isNil(y)) return car(x);

            y=cdr(y);
            z=cdr(z);
        }

        x = n<0? cadr(x) : cddr(x);
    }



    return NULL;
}
