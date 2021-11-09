#include <lisp.h>

void wrOpen(Context *CONTEXT_PTR, any ex, any x, outFrame *f)
{
   //NeedSymb(ex,x);
   if (isNil(x))
      f->fp = stdout;
   else {
      char *nm = (char *)malloc(pathSize(CONTEXT_PTR, x));

      pathString(CONTEXT_PTR, x,nm);
      if (nm[0] == '+') {
         if (!(f->fp = fopen(nm+1, "ab")))
            openErr(ex, nm);
      }
      else if (!(f->fp = fopen(nm, "wb")))
         openErr(ex, nm);

      free(nm);
   }
}
