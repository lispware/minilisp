#include <lisp.h>

void prin(Context *CONTEXT_PTR, any x)
{
    if (x == Nil)
    {
        printf("Nil");
        return;
    }

    if (!isNil(x))
    {
        if (isNum(x))
        {
            outNum(CONTEXT_PTR, x);
        }
        else if (x == T)
        {
            printf("T");
        }
        else if (getCARType(x) == TXT)
        {
            printLongTXT(CONTEXT_PTR, x);
        }
        else if (getCARType(x) == BIN_START)
        {
            printLongTXT(CONTEXT_PTR, x);

        }
        else
        {
            while (prin(CONTEXT_PTR, car(x)), !isNil(x = cdr(x)))
            {
                if (!isCell(x))
                {
                    prin(CONTEXT_PTR, x);
                    break;
                }
            }
        }
    }
}
