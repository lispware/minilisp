#include <lisp.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <tommath.h>
#include "../net.h"

void putStdoutNet(Context *CONTEXT_PTR, int c)
{
    char buf[1];
    buf[0] = (char)c;
    word current_client = (word)CONTEXT_PTR->OutFile;

    send(current_client, buf, 1, 0);
}
