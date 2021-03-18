#include "../../lisp.h"
#include "../platform.h"

void plt_thread_start(Context *CONTEXT_PTR, thread_func_t FUNC, int wait)
{
        FUNC(CONTEXT_PTR);
}
