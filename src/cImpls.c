#include "lisp.h"
#include "cell.h"

#if INTPTR_MAX == INT32_MAX
    #include "def32.d"
#elif INTPTR_MAX == INT64_MAX
    #include "def64.d"
#else
    #error "Unsupported bit width"
#endif

/* List length calculation */
inline uword length(any x)
{
   uword n;

   if (getCDRType(x) != PTR_CELL) return 0;
   if (cdr(x) == x || cdr(x) == 0) return 0;

   for (n = 0; x != Nil; x = cdr(x)) ++n;
   return n;
}

/* List interpreter */
inline any prog(any x)
{
   any y;

   do
   {
      y = EVAL(car(x));
   }
   while (Nil != (x = cdr(x)));

   return y;
}

inline any run(any x)
{
   any y;
   cell at;

   Push(at,val(At));
   do
   {
      y = EVAL(car(x));
   }
   while (Nil != (x = cdr(x)));
   val(At) = Pop(at);
   return y;
}




any doNot(any x) {
   any a;

   if (isNil(a = EVAL(cadr(x))))
      return T;
   val(At) = a;
   return Nil;
}

// (c...r 'lst) -> any
any doCar(any ex)
{
   any x = cdr(ex);
   x = EVAL(car(x));
   NeedLst(ex,x);
   return car(x);
}

any doCdr(any ex)
{
   any x = cdr(ex);
   x = EVAL(car(x));
   NeedLst(ex,x);
   return cdr(x);
}

any doCons(any x)
{
   any y;
   cell c1;

   x = cdr(x);
   Push(c1, y = cons(EVAL(car(x)),Nil));
   while (Nil != (cdr(x = cdr(x))))
   {
      y = cdr(y) = cons(EVAL(car(x)),Nil);
   }
   cdr(y) = EVAL(car(x));
   return Pop(c1);
}

any doRun(any x) {
   any y;
   cell c1;
   bindFrame *p;

   x = cdr(x),  data(c1) = EVAL(car(x)),  x = cdr(x);
   if (!isNum(data(c1))) {
      Save(c1);
      if (!isNum(y = EVAL(car(x))) || !(p = Env.bind))
         data(c1) = isSym(data(c1))? val(data(c1)) : run(data(c1));
      else {
         int cnt, n, i, j;
         //struct {  // bindFrame
         //   struct bindFrame *link;
         //   int i, cnt;
         //   struct {any sym; any val;} bnd[length(x)];
         //} f;

         bindFrame *f = allocFrame(length(x));

         x = cdr(x),  x = EVAL(car(x));
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
            for (p = Env.bind, j = n; ; p = p->link) {
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
next:       x = cdr(x);
         }
         f->link = Env.bind,  Env.bind = (bindFrame*)&f;
         data(c1) = isSym(data(c1))? val(data(c1)) : prog(data(c1));
         while (--f->cnt >= 0)
            val(f->bnd[f->cnt].sym) = f->bnd[f->cnt].val;
         Env.bind = f->link;
         do {
            for (p = Env.bind, i = n;  --i;  p = p->link);
            if (p->i < 0  &&  (p->i += cnt) == 0)
               for (i = p->cnt;  --i >= 0;) {
                  y = val(p->bnd[i].sym);
                  val(p->bnd[i].sym) = p->bnd[i].val;
                  p->bnd[i].val = y;
               }
         } while (--n);
      }
      drop(c1);
   }
   return data(c1);
}
