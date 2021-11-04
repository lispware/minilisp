#include <windows.h>
#include  <stdlib.h>
#include <stdio.h>
#include <winsock.h>
#include "../../lisp.h"
#include "../platform.h"


typedef struct _PACK
{
    Context *C;
    thread_func_t F;
} PACK;

DWORD WINAPI receive_cmds(LPVOID lpParam)
{
    PACK *P = (PACK*)lpParam;
    P->F(P->C);
    free(P);
    ExitThread(0);
    return 0;
}

void plt_thread_start(Context *CONTEXT_PTR, thread_func_t FUNC, int wait)
{
    DWORD thread;

   // PACK *P = (PACK*)calloc(sizeof(PACK), 1);
   // P->C = CONTEXT_PTR;
   // P->F = FUNC;

    // TODO - wait is not supported yet
    if (wait)
    {
        printf("Waiting for the completion of a thread is not supported yet\n");
    }
    _beginthread(FUNC, 0, CONTEXT_PTR);

    //CreateThread(NULL, 0,receive_cmds,(LPVOID)P, 0, &thread);
}
