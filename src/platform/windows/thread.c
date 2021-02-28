#include "../../lisp.h"
#include "../platform.h"

void thread_start(Context *CONTEXT_PTR, thread_func_t FUNC, int wait)
{
        FUNC(NULL);
}
