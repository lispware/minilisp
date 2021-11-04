#include <lisp.h>
#include "net.h"

void releaseSocket(struct _external* obj)
{
    pltClose(obj);
}
