#include "lisp.h"


#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#ifndef __NET_H__
#define __NET_H__
void getStdinNet(Context *CONTEXT_PTR);
any pltBind(Context *CONTEXT_PTR, word n);
any pltConnect(Context *CONTEXT_PTR, any ex);
any pltHttp(Context *CONTEXT_PTR, any ex);
any pltListen(Context *CONTEXT_PTR, word n);
any pltSocket(Context *CONTEXT_PTR, any ex);
void popIOFilesNet(Context *CONTEXT_PTR);
void pushIOFilesNet(Context *CONTEXT_PTR, inFrame *f, outFrame *fo);
void putStdoutNet(Context *CONTEXT_PTR, int c);
any doBind(Context *CONTEXT_PTR, any ex);
any doConnect(Context *CONTEXT_PTR, any ex);
any doHTTP(Context *CONTEXT_PTR, any ex);
any doListen(Context *CONTEXT_PTR, any ex);
any doSocket(Context *CONTEXT_PTR, any ex);
any doSocketClose(Context *CONTEXT_PTR, any ex);
void releaseSocket(struct _external* obj);
char *printSocket(Context *CONTEXT_PTR, struct _external* obj);
external * copySocket(Context *CONTEXT_PTR, external *ext);
int equalSocket(Context *CONTEXT_PTR, external*x, external*y);
void pltClose(struct _external* obj);

word GetThreadID();

#define NewExternalSocket(EXT_PARAM, FD) external *EXT_PARAM = (external *)malloc(sizeof(external));\
    EXT_PARAM->type = EXT_SOCKET;\
    EXT_PARAM->release = releaseSocket;\
    EXT_PARAM->print = printSocket;\
    EXT_PARAM->equal = equalSocket;\
    EXT_PARAM->copy = copySocket;\
    EXT_PARAM->pointer = (void*)(uword)FD;

#endif
#ifndef __THREAD_H__
#define __THREAD_H__


void copyHeap(Context *From, Context *To);
void copyBackupCell(cell *fromCell, cell * toCell);
void copyFixupCell(Context *From, Context *To, cell *fromCell, cell * toCell);
void copyRestoreCell(Context *From, Context *To, cell *fromCell, cell *toCell);
void copyHeap(Context *From, Context *To);
typedef void * (*thread_func_t)(void *);
void plt_thread_start(Context *CONTEXT_PTR, thread_func_t FUNC, int wait);
void plt_sleep(int ms);
any pltGetThreadId(Context *CONTEXT_PTR);

#endif

void putStdoutNet(Context *CONTEXT_PTR, int c)
{
    char buf[1];
    buf[0] = (unsigned char)c;
    word current_client = (word)CONTEXT_PTR->OutFile;

    send(current_client, buf, 1, 0);
}

any pltSocket(Context *CONTEXT_PTR, any ex)
{
    signal(SIGPIPE, SIG_IGN);

    uword n;
    any x,y;
    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;

    external *e = (external*)car(y);
    n = (uword)e->pointer;

    inFrame f;
    outFrame fo;

    f.fp = fo.fp = (FILE*)n;
    f.get = getStdinNet;
    fo.put = putStdoutNet;

    pushIOFilesNet(CONTEXT_PTR, &f, &fo);
    x = prog(CONTEXT_PTR, cddr(ex));
    popIOFilesNet(CONTEXT_PTR);

    e->release(e);
    car(y) = NULL;
    return Nil;
}

void getStdinNet(Context *CONTEXT_PTR)
{
    unsigned char buf[1];

    word current_client = (word)CONTEXT_PTR->InFile;

    read(current_client, buf, 1);
    CONTEXT_PTR->Chr = buf[0];
}

any pltListen(Context *CONTEXT_PTR, word n)
{
    int server_fd = (int)n;
    int new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                    (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    NewExternalSocket(e, new_socket);

    any r = cons(CONTEXT_PTR, Nil, Nil);
    setCARType(r, EXT);
    car(r) = (any)e;

    return r;
}

any pltBind(Context *CONTEXT_PTR, word n)
{
    cell c1;

    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        fprintf(stderr, "socket failed\n");
        return Nil;
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                &opt, sizeof(opt)))
    {
        fprintf(stderr, "setsockopt failed\n");
        return Nil;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( n );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, 
                sizeof(address))<0)
    {
        fprintf(stderr, "bind failed\n");
        return Nil;
    }

    NewExternalSocket(e, server_fd);

    any r = cons(CONTEXT_PTR, Nil, Nil);
    car(r) = (any)e;
    setCARType(r, EXT);

    return r;
}

void pltClose(struct _external* obj)
{
    if (obj->type != EXT_SOCKET)
    {
        fprintf(stderr, "Not a socket %p %d\n", obj, obj->type);
        exit(0);
    }

    close((uword)obj->pointer);
    free(obj);
}

void popIOFilesNet(Context *CONTEXT_PTR)
{

    CONTEXT_PTR->Chr = CONTEXT_PTR->Env.inFrames->next;
    CONTEXT_PTR->Env.get = CONTEXT_PTR->Env.inFrames->get;
    CONTEXT_PTR->InFile = (CONTEXT_PTR->Env.inFrames = CONTEXT_PTR->Env.inFrames->link)?  CONTEXT_PTR->Env.inFrames->fp : stdin;

    CONTEXT_PTR->Env.put = CONTEXT_PTR->Env.outFrames->put;
    CONTEXT_PTR->OutFile = (CONTEXT_PTR->Env.outFrames = CONTEXT_PTR->Env.outFrames->link)? CONTEXT_PTR->Env.outFrames->fp : stdout;
}

void pushIOFilesNet(Context *CONTEXT_PTR, inFrame *f, outFrame *fo)
{
    f->next = CONTEXT_PTR->Chr,  CONTEXT_PTR->Chr = 0;
    CONTEXT_PTR->InFile = f->fp;
    f->get = CONTEXT_PTR->Env.get,  CONTEXT_PTR->Env.get = getStdinNet;
    f->link = CONTEXT_PTR->Env.inFrames,  CONTEXT_PTR->Env.inFrames = f;

    CONTEXT_PTR->OutFile = fo->fp;
    fo->put = CONTEXT_PTR->Env.put,  CONTEXT_PTR->Env.put = putStdoutNet;
    fo->link = CONTEXT_PTR->Env.outFrames,  CONTEXT_PTR->Env.outFrames = fo;
}

any pltConnect(Context *CONTEXT_PTR, any ex)
{
    char *targetHostName;
    uword n;
    any x,y;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
    {
        targetHostName = (char*)malloc(10);
        strcpy(targetHostName, "127.0.0.1");
    }
    else if (isSym(y))
    {
        int len = symBytes(CONTEXT_PTR, y);
        targetHostName = (char*)malloc(len + 1);
        sym2str(CONTEXT_PTR, y, targetHostName);
    }

    x = cdr(x);
    y = EVAL(CONTEXT_PTR, car(x));

    NeedNum(ex,y);
    n = mpz_get_ui(num(y));

    cell c1;

    word client_socket;
    int valread;
    struct sockaddr_in serv_addr;

    // Creating socket file descriptor
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(n);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, targetHostName, &serv_addr.sin_addr) <= 0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return Nil;
    }

    free(targetHostName);

    if (connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return Nil;
    }

    NewExternalSocket(e, client_socket);

    any r = cons(CONTEXT_PTR, Nil, Nil);
    setCARType(r, EXT);
    car(r) = (any)e;

    return r;
}

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
    MP_INT *n = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(n);
    mpz_set_ui(n, pthread_self());

    NewNumber( n, r);
    return r;
}

word GetThreadID()
{
    return (word)pthread_self();
}

int nanosleep(const struct timespec *req, struct timespec *rem);

void plt_sleep(int milliseconds)
{
    usleep(milliseconds*1000);
}

any doOs(Context *CONTEXT_PTR, any ex)
{
    return mkStr(CONTEXT_PTR, "Unix");
}
