#include <lisp.h>
#include <tommath.h>
#include "../net.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock.h>

static WSADATA wsaData;
void initializeWindowsSockets()
{
    if (wsaData.wVersion)
    {
        // wsaData is already initialized
        return;
    }

    int ret = WSAStartup(0x101, &wsaData); // use highest version of winsock avalible
    if(ret != 0)
    {
        fprintf(stderr, "WSAStartup failed %d\n", ret);
        exit(0);
    }
}

