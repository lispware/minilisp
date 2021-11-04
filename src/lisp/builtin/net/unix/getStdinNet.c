#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <lisp.h>
#include <tommath.h>
#include "../net.h"

void getStdinNet(Context *CONTEXT_PTR)
{
    char buf[1];

    word current_client = (word)CONTEXT_PTR->InFile;


    read(current_client, buf, 1);
    CONTEXT_PTR->Chr = buf[0];
}
