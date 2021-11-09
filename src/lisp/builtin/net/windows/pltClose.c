#include <lisp.h>
#include <tommath.h>
#include "../net.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock.h>

void pltClose(struct _external* obj)
{
    if (obj->type != EXT_SOCKET)
    {
        fprintf(stderr, "Not a socket\n");
        exit(0);
    }
    closesocket((int)obj->pointer);
    free(obj);
}
