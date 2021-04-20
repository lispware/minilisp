#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "../lisp.h"

#if INTPTR_MAX == INT32_MAX
    #include "../def32.d"
#elif INTPTR_MAX == INT64_MAX
    #include "../def64.d"
#else
    #error "Unsupported bit width"
#endif


typedef void * (*thread_func_t)(void *);


void plt_thread_start(Context *CONTEXT_PTR, thread_func_t FUNC, int wait);

void plt_sleep(int ms);

any plt_listen(Context *, word);
any plt_bind(Context *, word);

#endif
