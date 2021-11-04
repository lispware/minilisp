#include <lisp.h>
#include <tommath.h>
#include "net.h"

any doConnect(Context *CONTEXT_PTR, any ex)
{
    return pltConnect(CONTEXT_PTR, ex);
}
