#include <lisp.h>
#include <stdio.h>
#include <stdlib.h>
#include "net.h"

int equalSocket(Context *CONTEXT_PTR, external*x, external*y)
{
    if (x->type != EXT_SOCKET)
    {
        fprintf(stderr, "LHS is not socket\n");
        return 1;
    }

    if (y->type != EXT_SOCKET)
    {
        fprintf(stderr, "RHS is not socket\n");
        return 1;
    }

    return x == y;
}
