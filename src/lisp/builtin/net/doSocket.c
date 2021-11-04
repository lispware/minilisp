#include <lisp.h>
#include <tommath.h>
#include "net.h"

any doSocket(Context *CONTEXT_PTR, any ex)
{
    return pltSocket(CONTEXT_PTR, ex);
}
