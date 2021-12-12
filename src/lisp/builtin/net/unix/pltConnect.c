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
    n = mp_get_i32((mp_int*)y->car);

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

    external *e = NewExternalSocket(client_socket);

    any r = cons(CONTEXT_PTR, (any)e, Nil);
    setCARType(r, EXT);

    return r;
}
