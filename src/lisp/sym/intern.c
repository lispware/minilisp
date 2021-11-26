#include <lisp.h>

any intern(Context *CONTEXT_PTR, any sym, any tree[2])
{
   any nm, x;
   word n;

   return internBin(CONTEXT_PTR, sym, tree);

}
