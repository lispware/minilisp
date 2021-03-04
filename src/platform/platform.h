#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "../lisp.h"


typedef void * (*thread_func_t)(void *);


void thread_start(Context *CONTEXT_PTR, thread_func_t FUNC, int wait);

#endif
