#include <lisp.h>

int MEMS;
any Mem;

Context LISP_CONTEXT;

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



    loadAll(CONTEXT_PTR, NULL);
    while (!feof(stdin))
        load(CONTEXT_PTR, NULL, ':', Nil);
    bye(0);

    return 0;
}
