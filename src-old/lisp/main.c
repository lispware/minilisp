#include <lisp.h>

int MEMS;
any Mem;

Context LISP_CONTEXT;

int PUSH_POP=0;
void ppp(Context*CONTEXT_PTR, char *m, cell c)
{
    //for (int i = 0; i < PUSH_POP; i++) printf(" ");
    //printf("c.car=%p c.cdr=%p Env->stack=%p %s", (c).car, (c).cdr, CONTEXT_PTR->Env.stack, m);
}

int main(int argc, char *av[])
{
    Context *CONTEXT_PTR = &LISP_CONTEXT;
    setupBuiltinFunctions(&CONTEXT_PTR->Mem);
    initialize_context(CONTEXT_PTR);
    av++;
    CONTEXT_PTR->AV = av;

    CONTEXT_PTR->InFile = stdin, CONTEXT_PTR->Env.get = getStdin;
    CONTEXT_PTR->OutFile = stdout, CONTEXT_PTR->Env.put = putStdout;
    CONTEXT_PTR->ApplyArgs = Nil;
    CONTEXT_PTR->ApplyBody = Nil;


    //fprintf(stderr, "main thread id = %p\n", pthread_self());
    fprintf(stderr, "Env->stack=%p\n", CONTEXT_PTR->Env.stack);

    loadAll(CONTEXT_PTR, NULL);
    while (!feof(stdin))
        load(CONTEXT_PTR, NULL, ':', Nil);
    bye(0);

    return 0;
}
