#include <lisp.h>
#include <tommath.h>
#include "../net.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock.h>

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

    external *e = (external *)malloc(sizeof(external));
    e->type = EXT_SOCKET;
    e->release = releaseSocket;
    e->print = printSocket;
    e->pointer = (void*)sock;

    printf("BIND e = %p POINTER =  %p\n", e, e->pointer);

    any r = cons(CONTEXT_PTR, (any)e, Nil);
    setCARType(r, EXT);

    return r;
}