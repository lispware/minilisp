#include <lisp.h>
#include <tommath.h>
#include "../net.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock.h>

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

    external *e = (external *)malloc(sizeof(external));
    e->type = EXT_SOCKET;
    e->release = releaseSocket;
    e->print = printSocket;
    e->pointer = (void*)client;

    any r = cons(CONTEXT_PTR, (any)e, Nil);
    setCARType(r, EXT);

    return r;
}
