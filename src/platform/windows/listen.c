#include "../../lisp.h"
#include "../platform.h"

any plt_listen(Context *CONTEXT_PTR, word n)
{
    cell c1;

    Push(c1, mkNum(CONTEXT_PTR, n));

    return Pop(c1);
}

