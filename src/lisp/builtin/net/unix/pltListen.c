#include <lisp.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <tommath.h>
#include "../net.h"

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

    external *e = (external *)malloc(sizeof(external));
    e->type = EXT_SOCKET;
    e->release = releaseSocket;
    e->print = printSocket;
    e->pointer = (void*)(uword)new_socket;

    any r = cons(CONTEXT_PTR, (any)e, Nil);
    setCARType(r, EXT);

    return r;
}
