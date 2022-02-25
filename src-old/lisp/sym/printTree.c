#include <lisp.h>

static void p(Context *CONTEXT_PTR, any nm)
{
    int i, c;
    uword w;

    c = getByte1(CONTEXT_PTR, &i, &w, &nm);
    do
    {
        if (c == '"'  ||  c == '\\')
        {
            printf("\\");
        }
        printf("%c", c);
    }
   while (c = getByte(CONTEXT_PTR, &i, &w, &nm));
}

static void printTab(int tab)
{
    for(int i = 0; i < tab; i++) printf("  ");
}


void printTree(Context *CONTEXT_PTR, any r, int tab, int t)
{
    printTab(tab);

    if (t == 0 ) printf("root ");
    else if (t == 1) printf("CAR ");
    else printf("CDR ");

    if (isNil(r))
    {
        printf("<Nil>\n");
        return;
    }
    else if (isSym(r))
    {
        p(CONTEXT_PTR, r);
        printf(" %p\n", r);
        return;
    }
    else
    {
        printf("%p\n", r);
    }

    printTree(CONTEXT_PTR, car(r), tab + 1, 1);
    printTree(CONTEXT_PTR, cdr(r), tab + 1, 2);
}
