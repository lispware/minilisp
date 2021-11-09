#include <lisp.h>

void rdOpen(Context *CONTEXT_PTR, any ex, any x, inFrame *f)
{
    //NeedSymb(ex,x); // TODO WHAT IS THIS ABOUT?
    if (isNil(x))
    {
        f->fp = stdin;
    }
    else
    {
        int ps = pathSize(CONTEXT_PTR, x);
        char *nm = (char*)malloc(ps);

        pathString(CONTEXT_PTR, x,nm);

        if (!(f->fp = fopen(nm, "rb")))
        {
            openErr(ex, nm);
        }

        free(nm);
    }
}
