#include <lisp.h>
#include <tommath.h>
#include "../net.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock.h>

void getStdinNet(Context *CONTEXT_PTR)
{
    char buf[1];

    SOCKET current_client = (SOCKET)CONTEXT_PTR->InFile;

    int res = recv(current_client,buf,1,0);
    CONTEXT_PTR->Chr = buf[0];
}
