#include <lisp.h>
#include <tommath.h>
#include "../net.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock.h>

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
    else if (getCARType(y) == BIN_START)
    {
        int len = symBytes(CONTEXT_PTR, y);
        targetHostName = (char*)malloc(len + 1);
        sym2str(CONTEXT_PTR, y, targetHostName);
    }

    x = cdr(x);
    y = EVAL(CONTEXT_PTR, car(x));

    NeedNum(ex,y);
    n = mp_get_i32((mp_int*)y->car);

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

    external *e = (external *)malloc(sizeof(external));
    e->type = EXT_SOCKET;
    e->release = releaseSocket;
    e->print = printSocket;
    e->pointer = (void*)client_socket;

    any r = cons(CONTEXT_PTR, (any)e, Nil);
    setCARType(r, EXT);

    return r;
}
