#include <lisp.h>

// (eval 'any ['cnt ['lst]]) -> any
any doEval(Context *CONTEXT_PTR, any x)
{
   any y;
   cell c1;
   bindFrame *p;


   x = cdr(x),  Push(c1, EVAL(CONTEXT_PTR, car(x))),  x = cdr(x);
   if (!isNum(y = EVAL(CONTEXT_PTR, car(x))) || !(p = CONTEXT_PTR->Env.bind))
      data(c1) = EVAL(CONTEXT_PTR, data(c1));
   else {
      int cnt, n, i, j;
      // struct {  // bindFrame
      //    struct bindFrame *link;
      //    int i, cnt;
      //    struct {any sym; any val;} bnd[length(CONTEXT_PTR, x)];
      // } f;

      bindFrame *f = allocFrame(length(CONTEXT_PTR, x));

      x = cdr(x),  x = EVAL(CONTEXT_PTR, car(x));
      j = cnt = (int)unBox(y);
      n = f->i = f->cnt = 0;
      do {
         ++n;
         if ((i = p->i) <= 0  &&  (p->i -= cnt, i == 0)) {
            for (i = 0;  i < p->cnt;  ++i) {
               y = val(p->bnd[i].sym);
               val(p->bnd[i].sym) = p->bnd[i].val;
               p->bnd[i].val = y;
            }
            if (p->cnt  &&  p->bnd[0].sym == At  &&  !--j)
               break;
         }
      } while (p = p->link);
      while (isCell(x)) {
         for (p = CONTEXT_PTR->Env.bind, j = n; ; p = p->link) {
            if (p->i < 0)
               for (i = 0;  i < p->cnt;  ++i) {
                  if (p->bnd[i].sym == car(x)) {
                     f->bnd[f->cnt].val = val(f->bnd[f->cnt].sym = car(x));
                     val(car(x)) = p->bnd[i].val;
                     ++f->cnt;
                     goto next;
                  }
               }
            if (!--j)
               break;
         }
next:    x = cdr(x);
      }
      f->link = CONTEXT_PTR->Env.bind,  CONTEXT_PTR->Env.bind = (bindFrame*)&f;
      data(c1) = EVAL(CONTEXT_PTR, data(c1));
      while (--f->cnt >= 0)
         val(f->bnd[f->cnt].sym) = f->bnd[f->cnt].val;
      CONTEXT_PTR->Env.bind = f->link;
      do {
         for (p = CONTEXT_PTR->Env.bind, i = n;  --i;  p = p->link);
         if (p->i < 0  &&  (p->i += cnt) == 0)
            for (i = p->cnt;  --i >= 0;) {
               y = val(p->bnd[i].sym);
               val(p->bnd[i].sym) = p->bnd[i].val;
               p->bnd[i].val = y;
            }
      } while (--n);
   }
   return Pop(c1);
}
