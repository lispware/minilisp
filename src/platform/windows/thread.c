#include "../../lisp.h"
#include "../platform.h"

void plt_thread_start(Context *CONTEXT_PTR, thread_func_t FUNC, int wait)
{
    // TODO - wait is not supported yet
    if (wait)
    {
        printf("Waiting for the completion of a thread is not supported yet\n");
    }
    _beginthread(FUNC, 0, CONTEXT_PTR);
}
