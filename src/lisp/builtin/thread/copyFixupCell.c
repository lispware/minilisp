#include "thread.h"
#include <tommath.h>

void copyFixupCell(Context *From, Context *To, cell *fromCell, cell * toCell)
{
    CellPartType carType;
    carType = getCARType(toCell);

    if (carType == EXT)
    {
        external *e = (external*)fromCell->car;
        if (e) toCell->car = (any)e->copy(From, e);
        else toCell->car = fromCell->car;
    }
    //else if (carType == NUM)
    //{
    //    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    //    mp_err _mp_error = mp_init(n); // TODO handle the errors appropriately
    //    _mp_error = mp_copy((mp_int*)fromCell->car, n);
    //    toCell->car = (any)n;

    //    //int len;
    //    //_mp_error = mp_radix_size((mp_int*)fromCell->car, 10, &len);
    //    //char *buf = (char*)malloc(len);
    //    //_mp_error = mp_to_radix((mp_int*)fromCell->car, buf, len, NULL, 10);
    //    //    printf("COPYING NUMBER %s\n", buf);
    //    //free(buf);
    //}
    else if (carType == FUNC || carType == BIN)
    {
        toCell->car = fromCell->car;
    }
    else
    {
        if (fromCell->car != 0)
        {
            toCell->car = fromCell->car->meta.ptr;
        }
        else
        {
            toCell->car = fromCell->car;
        }
    }

    if (fromCell->cdr != 0)
    {
        toCell->cdr = fromCell->cdr->meta.ptr;
    }
    else
    {
        toCell->cdr = fromCell->cdr;
    }
}
