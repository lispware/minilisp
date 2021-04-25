#include "../../lisp.h"
#include "../platform.h"

#include "../../cell.h"

#if INTPTR_MAX == INT32_MAX
    #include "../../def32.d"
#elif INTPTR_MAX == INT64_MAX
    #include "../../def64.d"
#else
    #error "Unsupported bit width"
#endif

#include <windows.h>
#include  <stdlib.h>
#include <stdio.h>
#include <winsock.h>

void getStdinNet(Context *CONTEXT_PTR)
{
    char buf[1];

    SOCKET current_client = (SOCKET)CONTEXT_PTR->InFile;

    int res = recv(current_client,buf,1,0);
    CONTEXT_PTR->Chr = buf[0];
}

void putStdoutNet(Context *CONTEXT_PTR, int c)
{
    char buf[1];
    buf[0] = (char)c;
    SOCKET current_client = (SOCKET)CONTEXT_PTR->OutFile;

    send(current_client,buf,1,0);
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

void popIOFilesNet(Context *CONTEXT_PTR)
{

    CONTEXT_PTR->Chr = CONTEXT_PTR->Env.inFrames->next;
    CONTEXT_PTR->Env.get = CONTEXT_PTR->Env.inFrames->get;
    CONTEXT_PTR->InFile = (CONTEXT_PTR->Env.inFrames = CONTEXT_PTR->Env.inFrames->link)?  CONTEXT_PTR->Env.inFrames->fp : stdin;

    CONTEXT_PTR->Env.put = CONTEXT_PTR->Env.outFrames->put;
    CONTEXT_PTR->OutFile = (CONTEXT_PTR->Env.outFrames = CONTEXT_PTR->Env.outFrames->link)? CONTEXT_PTR->Env.outFrames->fp : stdout;
}


any plt_bind(Context *CONTEXT_PTR, word n)
{

    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    SOCKET sock;
    WSADATA wsaData;
    SOCKADDR_IN server;
    int ret = WSAStartup(0x101,&wsaData); // use highest version of winsock avalible

    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(n); // listen on telnet port 23

    // create our socket
    sock=socket(AF_INET,SOCK_STREAM,0);

    if(sock == INVALID_SOCKET)
    {
        return 0;
    }

    // bind our socket to a port(port 123)
    if( bind(sock,(SOCKADDR*)&server,sizeof(server)) !=0 )
    {
        return 0;
    }

    cell c1;
    Push(c1, mkNum(CONTEXT_PTR, sock));
    return Pop(c1);
}

any plt_listen(Context *CONTEXT_PTR, word n)
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

    SOCKADDR_IN from;
    int fromlen = sizeof(from);

    // accept connections
    client = accept(server_fd,(struct SOCKADDR*)&from,&fromlen);


    cell c1;

    Push(c1, mkNum(CONTEXT_PTR, client));

    return Pop(c1);
}


any plt_socket(Context *CONTEXT_PTR, any ex)
{
    uword n;
    any x,y;
    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = unBox(y);

    inFrame f;
    outFrame fo;

    // printf("----------->%d\n", n);
    // char buf[100];
    // read(n, buf, 100);
    // printf("===> %s\n", buf);

    f.fp = fo.fp = (FILE*)n;
    f.get = getStdinNet;
    fo.put = putStdoutNet;

    pushIOFilesNet(CONTEXT_PTR, &f, &fo);
    x = prog(CONTEXT_PTR, cddr(ex));
    popIOFilesNet(CONTEXT_PTR);

    return Nil;
}

any plt_http(Context *CONTEXT_PTR, any ex)
{
    uword n;
    any x,y;
    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = unBox(y);

    inFrame f;
    outFrame fo;

    // printf("----------->%d\n", n);
    // char buf[100];
    // read(n, buf, 100);
    // printf("===> %s\n", buf);

    f.fp = fo.fp = (FILE*)n;
    f.get = getStdinNet;
    fo.put = putStdoutNet;

    pushIOFilesNet(CONTEXT_PTR, &f, &fo);
    x = prog(CONTEXT_PTR, cddr(ex));
    popIOFilesNet(CONTEXT_PTR);

    return Nil;
}

any plt_connect(Context *CONTEXT_PTR, any ex)
{
    return Nil;
}

any plt_socket_close(Context *CONTEXT_PTR, any ex)
{
    uword n;
    any x,y;
    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = unBox(y);


    printf("CLOSE SOCKET\n");

    close(n);

    return Nil;
}
