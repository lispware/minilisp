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

// (for sym 'num ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
// (for sym|(sym2 . sym) 'lst ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
// (for (sym|(sym2 . sym) 'any1 'any2 [. prg]) ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
any doFor(any x) {
   any y, body, cond, a;
   cell c1;
   // struct {  // bindFrame
   //    struct bindFrame *link;
   //    int i, cnt;
   //    struct {any sym; any val;} bnd[2];
   // } f;

   bindFrame *f = allocFrame(2);

   f->link = Env.bind,  Env.bind = f;
   f->i = 0;
   if (!isCell(y = car(x = cdr(x))) || !isCell(cdr(y))) {
      if (!isCell(y)) {
         f->cnt = 1;
         f->bnd[0].sym = y;
         f->bnd[0].val = val(y);
      }
      else {
         f->cnt = 2;
         f->bnd[0].sym = cdr(y);
         f->bnd[0].val = val(cdr(y));
         f->bnd[1].sym = car(y);
         f->bnd[1].val = val(car(y));
         val(f->bnd[1].sym) = Zero;
      }
      y = Nil;
      x = cdr(x),  Push(c1, EVAL(car(x)));
      if (isNum(data(c1)))
         f->bnd[0].sym->cdr  = mkNum(0);
      body = x = cdr(x);
      for (;;) {
         if (isNum(data(c1))) {
            word l,r;
            l = (word) f->bnd[0].sym->cdr->car;
            r = num(data(c1)->car);
            if ( l >= r ) 
               break;
            f->bnd[0].sym->cdr->car = (any)(l + 1);
         }
         else {
            if (Nil == (data(c1)))
               break;
            val(f->bnd[0].sym) = car(data(c1));
            if (Nil == (data(c1) = cdr(data(c1))))
               data(c1) = Nil;
         }
         if (f->cnt == 2)
            val(f->bnd[1].sym) = (any)(num(val(f->bnd[1].sym)) + 4);
         do {
            if (!isNum(y = car(x))) {
               if (isSym(y))
                  y = val(y);
               else if (isNil(car(y))) {
                  y = cdr(y);
                  if (isNil(a = EVAL(car(y)))) {
                     y = prog(cdr(y));
                     goto for1;
                  }
                  val(At) = a;
                  y = Nil;
               }
               else if (car(y) == T) {
                  y = cdr(y);
                  if (!isNil(a = EVAL(car(y)))) {
                     val(At) = a;
                     y = prog(cdr(y));
                     goto for1;
                  }
                  y = Nil;
               }
               else
                  y = evList(y);
            }
         } while (Nil != (x = cdr(x)));
         x = body;
      }
   for1:
      drop(c1);
      if (f->cnt == 2)
         val(f->bnd[1].sym) = f->bnd[1].val;
      val(f->bnd[0].sym) = f->bnd[0].val;
      Env.bind = f->link;
      return y;
   }
   if (!isCell(car(y))) {
      f->cnt = 1;
      f->bnd[0].sym = car(y);
      f->bnd[0].val = val(car(y));
   }
   else {
      f->cnt = 2;
      f->bnd[0].sym = cdar(y);
      f->bnd[0].val = val(cdar(y));
      f->bnd[1].sym = caar(y);
      f->bnd[1].val = val(caar(y));
      val(f->bnd[1].sym) = Zero;
   }
   y = cdr(y);
   val(f->bnd[0].sym) = EVAL(car(y));
   y = cdr(y),  cond = car(y),  y = cdr(y);
   Push(c1,Nil);
   body = x = cdr(x);
   for (;;) {
      if (f->cnt == 2)
         val(f->bnd[1].sym) = (any)(num(val(f->bnd[1].sym)) + 4);
      if (isNil(a = EVAL(cond)))
         break;
      val(At) = a;
      do {
         if (!isNum(data(c1) = car(x))) {
            if (isSym(data(c1)))
               data(c1) = val(data(c1));
            else if (isNil(car(data(c1)))) {
               data(c1) = cdr(data(c1));
               if (isNil(a = EVAL(car(data(c1))))) {
                  data(c1) = prog(cdr(data(c1)));
                  goto for2;
               }
               val(At) = a;
               data(c1) = Nil;
            }
            else if (car(data(c1)) == T) {
               data(c1) = cdr(data(c1));
               if (!isNil(a = EVAL(car(data(c1))))) {
                  val(At) = a;
                  data(c1) = prog(cdr(data(c1)));
                  goto for2;
               }
               data(c1) = Nil;
            }
            else
               data(c1) = evList(data(c1));
         }
      } while (isCell(x = cdr(x)));
      if (isCell(y))
         val(f->bnd[0].sym) = prog(y);
      x = body;
   }
for2:
   if (f->cnt == 2)
      val(f->bnd[1].sym) = f->bnd[1].val;
   val(f->bnd[0].sym) = f->bnd[0].val;
   Env.bind = f->link;
   return Pop(c1);
}


// (setq var 'any ..) -> any
any doSetq(any ex)
{
    any x, y;

    x = cdr(ex);
    do
    {
        y = car(x),  x = cdr(x);
        NeedVar(ex,y);
        // CheckVar(ex,y); - TODO - what is this for?
        val(y) = EVAL(car(x));
    }
    while (Nil != (x = cdr(x)));

    return val(y);
}
