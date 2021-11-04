#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <lisp.h>
#include <tommath.h>

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

typedef void * (*thread_func_t)(void *);

void plt_thread_start(Context *CONTEXT_PTR, thread_func_t FUNC, int wait);

struct thread_info {    /* Used as argument to thread_start() */
    pthread_t thread_id;        /* ID returned by pthread_create() */
    int	     thread_num;       /* Application-defined thread # */
    char     *argv_string;      /* From command-line argument */
};

void plt_thread_start(Context *CONTEXT_PTR, thread_func_t FUNC, int wait)
{
    int s;
    pthread_t tid;
    pthread_attr_t attr;
    void *res;

    s = pthread_attr_init(&attr);
    if (s != 0)
        handle_error_en(s, "pthread_attr_init");
    s = pthread_create(&tid, &attr, FUNC, CONTEXT_PTR);
    
    if (wait)
    {
        s = pthread_join(tid, &res);
        if (s != 0)
            handle_error_en(s, "pthread_join");
    }
}

any pltGetThreadId(Context *CONTEXT_PTR)
{
    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    mp_err _mp_error = mp_init(n); // TODO handle the errors appropriately
    mp_set(n, pthread_self());

    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)n;
    r->meta.type.parts[0] = NUM;
    return r;
}
