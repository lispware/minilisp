#include <lisp.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../net.h"

any pltSocket(Context *CONTEXT_PTR, any ex)
{
    signal(SIGPIPE, SIG_IGN);

    uword n;
    any x,y;
    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;

    external *e = (external*)y->car;
    n = (uword)e->pointer;

    inFrame f;
    outFrame fo;

    f.fp = fo.fp = (FILE*)n;
    f.get = getStdinNet;
    fo.put = putStdoutNet;

    pushIOFilesNet(CONTEXT_PTR, &f, &fo);
    x = prog(CONTEXT_PTR, cddr(ex));
    popIOFilesNet(CONTEXT_PTR);

    e->release(e);
    y->car = NULL;
    return Nil;
}
