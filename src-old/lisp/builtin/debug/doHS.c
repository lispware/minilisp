#include <lisp.h>


any doHS(Context *CONTEXT_PTR, any ignore)
{
    gc(CONTEXT_PTR, CELLS);
    getHeapSize(CONTEXT_PTR);
    return Nil;
}
