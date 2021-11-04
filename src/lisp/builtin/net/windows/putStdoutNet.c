#include <lisp.h>
#include <tommath.h>
#include "../net.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock.h>

void putStdoutNet(Context *CONTEXT_PTR, int c)
{
    char buf[1];
    buf[0] = (char)c;
    SOCKET current_client = (SOCKET)CONTEXT_PTR->OutFile;

    send(current_client,buf,1,0);
}
