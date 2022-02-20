#include "lisp.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock.h>

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
// THIS IS FROM lisp/builtin/net/windows/putStdoutNet.c

void putStdoutNet(Context *CONTEXT_PTR, int c)
{
    unsigned char buf[1];
    buf[0] = (unsigned char)c;
    SOCKET current_client = (SOCKET)CONTEXT_PTR->OutFile;

    send(current_client,buf,1,0);
}
// THIS IS FROM lisp/builtin/net/windows/pltClose.c

void pltClose(struct _external* obj)
{
    if (obj->type != EXT_SOCKET)
    {
        fprintf(stderr, "Not a socket\n");
        exit(0);
    }
    closesocket((uword)obj->pointer);
    free(obj);
}
// THIS IS FROM lisp/builtin/net/windows/pltBind.c

void initializeWindowsSockets();

any pltBind(Context *CONTEXT_PTR, word n)
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    initializeWindowsSockets();

    SOCKADDR_IN server;
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(n); // listen on telnet port 23

    // create our socket
    SOCKET sock=socket(AF_INET,SOCK_STREAM,0);

    if(sock == INVALID_SOCKET)
    {
        return Nil;
    }

    // bind our socket to a port(port 123)
    if( bind(sock,(SOCKADDR*)&server,sizeof(server)) !=0 )
    {
        return Nil;
    }

    NewExternalSocket(e, sock);

    any r = cons(CONTEXT_PTR, Nil, Nil);
    car(r) = (any)e;
    setCARType(r, EXT);

    return r;
}
// THIS IS FROM lisp/builtin/net/windows/pltListen.c

any pltListen(Context *CONTEXT_PTR, word n)
{
    int server_fd = (int)n;
    int new_socket, valread;

    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if(listen(server_fd,5) != 0)
    {
        return 0;
    }

    SOCKET client;

    //SOCKADDR_IN from;
    SOCKADDR from;
    int fromlen = sizeof(from);

    // accept connections
    client = accept(server_fd, &from, &fromlen);

    NewExternalSocket(e, client);

    any r = cons(CONTEXT_PTR, Nil, Nil);
    setCARType(r, EXT);
    car(r) = (any)e;

    return r;
}
// THIS IS FROM lisp/builtin/net/windows/getStdinNet.c

void getStdinNet(Context *CONTEXT_PTR)
{
    unsigned char buf[1];

    SOCKET current_client = (SOCKET)CONTEXT_PTR->InFile;

    int res = recv(current_client,buf,1,0);
    CONTEXT_PTR->Chr = buf[0];
}
// THIS IS FROM lisp/builtin/net/windows/pltSocket.c

any pltSocket(Context *CONTEXT_PTR, any ex)
{
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
    car(y) = NULL; // TODO -> this has to be understood more
    return Nil;
}
// THIS IS FROM lisp/builtin/net/windows/initializeWindowsSockets.c

static WSADATA wsaData;
void initializeWindowsSockets()
{
    if (wsaData.wVersion)
    {
        // wsaData is already initialized
        return;
    }

    int ret = WSAStartup(0x101, &wsaData); // use highest version of winsock avalible
    if(ret != 0)
    {
        fprintf(stderr, "WSAStartup failed %d\n", ret);
        exit(0);
    }
}

// THIS IS FROM lisp/builtin/net/windows/pltConnect.c

any pltConnect(Context *CONTEXT_PTR, any ex)
{
    initializeWindowsSockets();

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
    n = mp_get_i32(num(y));

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
    serv_addr.sin_addr.s_addr = inet_addr(targetHostName);


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
// THIS IS FROM lisp/builtin/net/windows/popIOFilesNet.c

void popIOFilesNet(Context *CONTEXT_PTR)
{

    CONTEXT_PTR->Chr = CONTEXT_PTR->Env.inFrames->next;
    CONTEXT_PTR->Env.get = CONTEXT_PTR->Env.inFrames->get;
    CONTEXT_PTR->InFile = (CONTEXT_PTR->Env.inFrames = CONTEXT_PTR->Env.inFrames->link)?  CONTEXT_PTR->Env.inFrames->fp : stdin;

    CONTEXT_PTR->Env.put = CONTEXT_PTR->Env.outFrames->put;
    CONTEXT_PTR->OutFile = (CONTEXT_PTR->Env.outFrames = CONTEXT_PTR->Env.outFrames->link)? CONTEXT_PTR->Env.outFrames->fp : stdout;
}
// THIS IS FROM lisp/builtin/net/windows/pushIOFilesNet.c

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
// THIS IS FROM lisp/builtin/thread/windows/thread.c

typedef void * (*thread_func_t)(void *);
void plt_thread_start(Context *CONTEXT_PTR, thread_func_t FUNC, int wait);

typedef struct _PACK
{
    Context *C;
    thread_func_t F;
} PACK;

DWORD WINAPI receive_cmds(LPVOID lpParam)
{
    PACK *P = (PACK*)lpParam;
    P->F(P->C);
    free(P);
    ExitThread(0);
    return 0;
}

void plt_thread_start(Context *CONTEXT_PTR, thread_func_t FUNC, int wait)
{
    DWORD thread;

   // PACK *P = (PACK*)calloc(sizeof(PACK), 1);
   // P->C = CONTEXT_PTR;
   // P->F = FUNC;

    // TODO - wait is not supported yet
    if (wait)
    {
        printf("Waiting for the completion of a thread is not supported yet\n");
    }
    _beginthread(FUNC, 0, CONTEXT_PTR);

    //CreateThread(NULL, 0,receive_cmds,(LPVOID)P, 0, &thread);
}

any pltGetThreadId(Context *CONTEXT_PTR)
{
    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    mp_err _mp_error = mp_init(n); // TODO handle the errors appropriately
    mp_set(n, GetCurrentThreadId());

    NewNumber(ext, n, r);
    return r;
}

word GetThreadID()
{
    return (word)GetCurrentThreadId();
}
// THIS IS FROM lisp/builtin/thread/windows/sleep.c

int nanosleep(const struct timespec *req, struct timespec *rem);

void plt_sleep(int milliseconds)
{
    Sleep(milliseconds);
}
