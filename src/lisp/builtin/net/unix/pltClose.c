#include <lisp.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../net.h"

void pltClose(struct _external* obj)
{
    if (obj->type != EXT_SOCKET)
    {
        fprintf(stderr, "Not a socket %d\n", obj->type);
        exit(0);
    }
    fprintf(stderr, "Releasing the socket %p\n", obj->pointer );

    close((uword)obj->pointer);
    free(obj);
}
