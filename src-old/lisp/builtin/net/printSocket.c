#include <lisp.h>
#include <stdio.h>
#include <stdlib.h>
#include "net.h"

char * printSocket(Context *CONTEXT_PTR, struct _external* obj)
{
    char *buf=(char *)malloc(256);
    sprintf(buf, "Socket %p\n", obj->pointer);
    return buf;
}

