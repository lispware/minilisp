#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "../../lisp.h"
#include "../platform.h"

any plt_bind(Context *CONTEXT_PTR, word n)
{
    cell c1;


    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( n );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, 
                sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    Push(c1, mkNum(CONTEXT_PTR, server_fd));
    return Pop(c1);
}

any plt_listen(Context *CONTEXT_PTR, word n)
{
    int server_fd = (int)n;
    int new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    printf ("START LISTEN %d\n",  n);

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
    printf ("ACCEPTED A CONNECTION %d\n",  n);

    cell c1;
    Push(c1, mkNum(CONTEXT_PTR, new_socket));
    return Pop(c1);
}

void getStdinNet(Context *CONTEXT_PTR)
{
    char buf[1];

    int current_client = (int)CONTEXT_PTR->InFile;

    read(current_client, buf, 1);
    CONTEXT_PTR->Chr = buf[0];
}

void putStdoutNet(Context *CONTEXT_PTR, int c)
{
    char buf[1];
    buf[0] = (char)c;
    int current_client = (int)CONTEXT_PTR->OutFile;

    send(current_client, buf, 1, 0);
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
    signal(SIGPIPE, SIG_IGN);
    cell c1;
    Push(c1, ex);

    uword n;
    any x,y;
    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = unBox(y);

    inFrame f;
    outFrame fo;


    f.fp = fo.fp = (FILE*)n;
    f.get = getStdinNet;
    fo.put = putStdoutNet;

    pushIOFilesNet(CONTEXT_PTR, &f, &fo);
    x = prog(CONTEXT_PTR, cddr(ex));
    popIOFilesNet(CONTEXT_PTR);

    Pop(c1);
    return Nil;
}

any plt_connect(Context *CONTEXT_PTR, any ex)
{

    uword n;
    any x,y;
    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = unBox(y);

    cell c1;

    int client_socket, valread;
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
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    

    ////////////////////////////
    inFrame f;
    outFrame fo;


    f.fp = fo.fp = (FILE*)client_socket;
    f.get = getStdinNet;
    fo.put = putStdoutNet;

    pushIOFilesNet(CONTEXT_PTR, &f, &fo);
    x = prog(CONTEXT_PTR, cddr(ex));
    popIOFilesNet(CONTEXT_PTR);

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
