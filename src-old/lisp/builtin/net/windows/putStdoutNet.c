#include <lisp.h>
#include "../net.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock.h>

void putStdoutNet(Context *CONTEXT_PTR, int c)
{
    unsigned char buf[1];
    buf[0] = (unsigned char)c;
    SOCKET current_client = (SOCKET)CONTEXT_PTR->OutFile;

    send(current_client,buf,1,0);
}
