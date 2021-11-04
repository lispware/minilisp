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

void pushIOFilesNet(Context *CONTEXT_PTR, inFrame *f, outFrame *fo)
{
    f->next = CONTEXT_PTR->Chr,  CONTEXT_PTR->Chr = 0;
    CONTEXT_PTR->InFile = f->fp;
    f->get = CONTEXT_PTR->Env.get,  CONTEXT_PTR->Env.get = getStdinNet;
    f->link = CONTEXT_PTR->Env.inFrames,  CONTEXT_PTR->Env.inFrames = f;

    CONTEXT_PTR->OutFile = fo->fp;
    fo->put = CONTEXT_PTR->Env.put,  CONTEXT_PTR->Env.put = putStdoutNet;
    fo->link = CONTEXT_PTR->Env.outFrames,  CONTEXT_PTR->Env.outFrames = fo;
}
