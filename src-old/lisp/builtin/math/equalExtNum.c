#include <lisp.h>
#include <stdio.h>
#include <stdlib.h>
#include <tommath.h>

int equalExtNum(Context *CONTEXT_PTR, external*x, external*y)
{
    if (x->type != EXT_NUM)
    {
        fprintf(stderr, "LHS is not number\n");
        return 1;
    }

    if (y->type != EXT_NUM)
    {
        fprintf(stderr, "RHS is not number\n");
        return 1;
    }

    return mp_cmp((mp_int*)x->pointer, (mp_int*)y->pointer);
}
