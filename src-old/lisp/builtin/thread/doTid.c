#include "thread.h"

any doTid(Context *CONTEXT_PTR, any ex)
{
    return pltGetThreadId(CONTEXT_PTR);
}
