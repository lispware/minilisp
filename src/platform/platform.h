#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "../lisp.h"


typedef void * (*thread_func_t)(void *);


void plt_thread_start(Context *CONTEXT_PTR, thread_func_t FUNC, int wait);

void plt_sleep(int ms);

any plt_listen(Context *, word);

#endif
