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
        fprintf(stderr, "Not a socket %p %d\n", obj, obj->type);
        exit(0);
    }

    close((uword)obj->pointer);
    free(obj);
}
