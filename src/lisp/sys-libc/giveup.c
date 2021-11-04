#include <lisp.h>

void giveup(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}
