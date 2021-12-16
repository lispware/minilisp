#include <lisp.h>

void print(Context *CONTEXT_PTR, any x)
{
    if (x == T)
    {
        CONTEXT_PTR->Env.put(CONTEXT_PTR, 'T');
        return;
    }

    if (x == Nil)
    {
        outString(CONTEXT_PTR, "Nil");
        return;
    }

    if (isNum(x))
    {
        //outNum(CONTEXT_PTR, x);
        //return;
    }
    if (isSym(x))
    {
        int quotedText = NULL != isIntern(CONTEXT_PTR, x, CONTEXT_PTR->Transient);
        if (quotedText) CONTEXT_PTR->Env.put(CONTEXT_PTR, '"');
        printLongTXT(CONTEXT_PTR, x);
        if (quotedText) CONTEXT_PTR->Env.put(CONTEXT_PTR, '"');
        return;
    }

    if (GetType(x) == EXT)
    {
        external *e = (external*)car(x);
        char *b = e->print(CONTEXT_PTR, e);
        outString(CONTEXT_PTR, b);
        free(b);
        return;
    }

    if (isCell(x))
    {
        CONTEXT_PTR->Env.put(CONTEXT_PTR, '(');
        print(CONTEXT_PTR, car(x));
        while (Nil != (x = cdr(x)))
        {
            CONTEXT_PTR->Env.put(CONTEXT_PTR, ' ');
            if (isCell(x))
            {
                print(CONTEXT_PTR, car(x));
            }
            else
            {
                print(CONTEXT_PTR, x);
            }
        }
        CONTEXT_PTR->Env.put(CONTEXT_PTR, ')');
        return;
    }

    if (GetType(x) == FUNC)
    {
        char buf[256];
        sprintf (buf, "C FUNCTION %p", (void*)x);
        outString(CONTEXT_PTR, buf);
        return;
    }

    fprintf (stderr, "TODO NOT A NUMBER %p %p\n", (void*)x, (void*)Nil);
    return;
}
