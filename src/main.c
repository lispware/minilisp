#include "lisp.h"

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
    CONTEXT_PTR->ApplyArgs = cons(CONTEXT_PTR, cons(CONTEXT_PTR, consSym(CONTEXT_PTR, Nil, Nil), Nil), Nil);
    CONTEXT_PTR->ApplyBody = cons(CONTEXT_PTR, Nil, Nil);

    loadAll(CONTEXT_PTR, NULL);
    while (!feof(stdin))
        load(CONTEXT_PTR, NULL, ':', Nil);

    return 0;
}
