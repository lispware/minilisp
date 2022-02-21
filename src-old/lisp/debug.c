#include <lisp.h>

void debugIndent(Context *CONTEXT_PTR)
{
    CONTEXT_PTR->Tab++;
}

void debugOutdent(Context *CONTEXT_PTR)
{
    CONTEXT_PTR->Tab--;
}

void debugLog(Context *CONTEXT_PTR, char *message)
{
    for(int i = 0; i < CONTEXT_PTR->Tab; i++) fprintf(stderr, "  ");
    fprintf(stderr, "%s\n", message);
}

void debugLogAny(Context *CONTEXT_PTR, char *message, any x)
{
    for(int i = 0; i < CONTEXT_PTR->Tab; i++) fprintf(stderr, "  ");
    fprintf(stderr, "%s", message);
    print(CONTEXT_PTR, x);
    printf("\n");
}
