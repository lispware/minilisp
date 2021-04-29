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
inline uword length(Context *CONTEXT_PTR, any x)
{
   uword n;

   if (getCDRType(x) != PTR_CELL) return 0;
   if (getCARType(x) != PTR_CELL) return 0;
   if (cdr(x) == x) return 0;

   for (n = 0; x != Nil; x = cdr(x)) ++n;
   return n;
}

/* List interpreter */
inline any prog(Context *CONTEXT_PTR, any x)
{
   any y;

   do
   {
      y = EVAL(CONTEXT_PTR, car(x));
   }
   while (Nil != (x = cdr(x)));

   return y;
}

inline any run(Context *CONTEXT_PTR, any x)
{
   any y;
   cell at;

   Push(at,val(At));
   do
   {
      y = EVAL(CONTEXT_PTR, car(x));
   }
   while (Nil != (x = cdr(x)));
   val(At) = Pop(at);
   return y;
}




any doNot(Context *CONTEXT_PTR, any x) {
   any a;

   if (isNil(a = EVAL(CONTEXT_PTR, cadr(x))))
      return T;
   val(At) = a;
   return Nil;
}

// (c...r 'lst) -> any
any doCar(Context *CONTEXT_PTR, any ex)
{
   any x = cdr(ex);
   x = EVAL(CONTEXT_PTR, car(x));
   NeedLst(ex,x);
   return car(x);
}

any doCdr(Context *CONTEXT_PTR, any ex)
{
   any x = cdr(ex);
   x = EVAL(CONTEXT_PTR, car(x));
   NeedLst(ex,x);
   return cdr(x);
}

any doCons(Context *CONTEXT_PTR, any x)
{
   any y;
   cell c1;

   x = cdr(x);
   Push(c1, y = cons(CONTEXT_PTR, EVAL(CONTEXT_PTR, car(x)),Nil));
   while (Nil != (cdr(x = cdr(x))))
   {
      y = cdr(y) = cons(CONTEXT_PTR, EVAL(CONTEXT_PTR, car(x)),Nil);
   }
   cdr(y) = EVAL(CONTEXT_PTR, car(x));
   return Pop(c1);
}

any doRun(Context *CONTEXT_PTR, any x) {
   any y;
   cell c1;
   bindFrame *p;

   x = cdr(x),  data(c1) = EVAL(CONTEXT_PTR, car(x)),  x = cdr(x);
   if (!isNum(data(c1))) {
      Save(c1);
      if (!isNum(y = EVAL(CONTEXT_PTR, car(x))) || !(p = CONTEXT_PTR->Env.bind))
         data(c1) = isSym(data(c1))? val(data(c1)) : run(CONTEXT_PTR, data(c1));
      else {
         int cnt, n, i, j;
         //struct {  // bindFrame
         //   struct bindFrame *link;
         //   int i, cnt;
         //   struct {any sym; any val;} bnd[length(x)];
         //} f;

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
next:       x = cdr(x);
         }
         f->link = CONTEXT_PTR->Env.bind,  CONTEXT_PTR->Env.bind = (bindFrame*)&f;
         data(c1) = isSym(data(c1))? val(data(c1)) : prog(CONTEXT_PTR, data(c1));
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
      drop(c1);
   }
   return data(c1);
}

// (for sym 'num ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
// (for sym|(sym2 . sym) 'lst ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
// (for (sym|(sym2 . sym) 'any1 'any2 [. prg]) ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
any doFor(Context *CONTEXT_PTR, any x) {
   any y, body, cond, a;
   cell c1;
   // struct {  // bindFrame
   //    struct bindFrame *link;
   //    int i, cnt;
   //    struct {any sym; any val;} bnd[2];
   // } f;

   bindFrame *f = allocFrame(2);

   f->link = CONTEXT_PTR->Env.bind,  CONTEXT_PTR->Env.bind = f;
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
      x = cdr(x),  Push(c1, EVAL(CONTEXT_PTR, car(x)));
      if (isNum(data(c1)))
         f->bnd[0].sym->cdr  = mkNum(CONTEXT_PTR, 0);
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
                  if (isNil(a = EVAL(CONTEXT_PTR, car(y)))) {
                     y = prog(CONTEXT_PTR, cdr(y));
                     goto for1;
                  }
                  val(At) = a;
                  y = Nil;
               }
               else if (car(y) == T) {
                  y = cdr(y);
                  if (!isNil(a = EVAL(CONTEXT_PTR, car(y)))) {
                     val(At) = a;
                     y = prog(CONTEXT_PTR, cdr(y));
                     goto for1;
                  }
                  y = Nil;
               }
               else
                  y = evList(CONTEXT_PTR, y);
            }
         } while (Nil != (x = cdr(x)));
         x = body;
      }
   for1:
      drop(c1);
      if (f->cnt == 2)
         val(f->bnd[1].sym) = f->bnd[1].val;
      val(f->bnd[0].sym) = f->bnd[0].val;
      CONTEXT_PTR->Env.bind = f->link;
      free(f);
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
   val(f->bnd[0].sym) = EVAL(CONTEXT_PTR, car(y));
   y = cdr(y),  cond = car(y),  y = cdr(y);
   Push(c1,Nil);
   body = x = cdr(x);
   for (;;) {
      if (f->cnt == 2)
         val(f->bnd[1].sym) = (any)(num(val(f->bnd[1].sym)) + 4);
      if (isNil(a = EVAL(CONTEXT_PTR, cond)))
         break;
      val(At) = a;
      do {
         if (!isNum(data(c1) = car(x))) {
            if (isSym(data(c1)))
               data(c1) = val(data(c1));
            else if (isNil(car(data(c1)))) {
               data(c1) = cdr(data(c1));
               if (isNil(a = EVAL(CONTEXT_PTR, car(data(c1))))) {
                  data(c1) = prog(CONTEXT_PTR, cdr(data(c1)));
                  goto for2;
               }
               val(At) = a;
               data(c1) = Nil;
            }
            else if (car(data(c1)) == T) {
               data(c1) = cdr(data(c1));
               if (!isNil(a = EVAL(CONTEXT_PTR, car(data(c1))))) {
                  val(At) = a;
                  data(c1) = prog(CONTEXT_PTR, cdr(data(c1)));
                  goto for2;
               }
               data(c1) = Nil;
            }
            else
               data(c1) = evList(CONTEXT_PTR, data(c1));
         }
      } while (isCell(x = cdr(x)));
      if (isCell(y))
         val(f->bnd[0].sym) = prog(CONTEXT_PTR, y);
      x = body;
   }
for2:
   if (f->cnt == 2)
      val(f->bnd[1].sym) = f->bnd[1].val;
   val(f->bnd[0].sym) = f->bnd[0].val;
   CONTEXT_PTR->Env.bind = f->link;
   free(f);
   return Pop(c1);
}


// (setq var 'any ..) -> any
any doSetq(Context *CONTEXT_PTR, any ex)
{
    any x, y;

    x = cdr(ex);
    do
    {
        y = car(x),  x = cdr(x);
        NeedVar(ex,y);
        // CheckVar(ex,y); - TODO - what is this for?
        val(y) = EVAL(CONTEXT_PTR, car(x));
    }
    while (Nil != (x = cdr(x)));

    return val(y);
}


// (link 'any ..) -> any
any doLink(Context *CONTEXT_PTR, any x)
{
    any y;

    if (!CONTEXT_PTR->Env.make)
    {
        makeError(x);
    }
    x = cdr(x);
    do
    {
        y = EVAL(CONTEXT_PTR, car(x));
        CONTEXT_PTR->Env.make = &cdr(*CONTEXT_PTR->Env.make = cons(CONTEXT_PTR, y, Nil));
    }
    while (Nil != (x = cdr(x)));
    return y;
}

// (make .. [(made 'lst ..)] .. [(link 'any ..)] ..) -> any
any doMake(Context *CONTEXT_PTR, any x)
{
    any *make, *yoke;
    cell c1;

    Push(c1, Nil);
    make = CONTEXT_PTR->Env.make;
    yoke = CONTEXT_PTR->Env.yoke;
    CONTEXT_PTR->Env.make = CONTEXT_PTR->Env.yoke = &data(c1);

    while (Nil != (x = cdr(x)))
    {
        if (isCell(car(x)))
        {
            evList(CONTEXT_PTR, car(x));
        }
    }
    CONTEXT_PTR->Env.yoke = yoke;
    CONTEXT_PTR->Env.make = make;
    return Pop(c1);
}

// (prin 'any ..) -> any
any doPrin(Context *CONTEXT_PTR, any x)
{
   any y = Nil;

   while (Nil != (x = cdr(x))  )
   {
      prin(CONTEXT_PTR, y = EVAL(CONTEXT_PTR, car(x)));
   }
   newline(CONTEXT_PTR);
   return y;
}

// (quote . any) -> any
any doQuote(Context *CONTEXT_PTR, any x)
{
    return cdr(x);
}


// int compareBIN(any x1, any x2)
// {
//     return 1;
//     do
//     {
//         CellPartType t1, t2;
//         t1 = getCARType(x1);
//         t2 = getCARType(x2);
//         if (t1 != BIN || t1 != t2) return 0;
//         if (x1->car != x2->car) return 0;
//         x1 = x1->cdr;
//         x2 = x2->cdr;
// 
//     }
//     while(x1 != Nil);
// 
//     return 1;
// }


any doEq(Context *CONTEXT_PTR, any x)
{
   cell c1;
    x = cdr(x);

    if (x == Nil)
    {
        return T;
    }

    Push(c1, EVAL(CONTEXT_PTR, car(x)));

    any v = EVAL(CONTEXT_PTR, car(x));
    CellPartType vt = getCARType(v);

    x = cdr(x);

    while(x != Nil)
    {
        any v2 = EVAL(CONTEXT_PTR, car(x));
        CellPartType t = getCARType(v2);
        if (t != vt) {drop(c1); return Nil;}

        if(t == NUM || t == TXT)
        {
            if (car(v2) != car(v)) {drop(c1); return Nil;}
        }
        else if (t == BIN_START)
        {
            any p1 = car(v);
            any p2 = car(v2);
            do
            {
                if (car(p1) != car(p2)) {drop(c1); return Nil;}
                p1 = cdr(p1);
                p2 = cdr(p2);
            }
            while (p1 != Nil);

            drop(c1);
            return p2 == Nil? T : Nil;
        }
        else
        {
              if ( v != v2)
              {
                 drop(c1);
                 return Nil;
              }
        }

        x = cdr(x);
    }

    drop(c1);
    return T;
}

// (== 'any ..) -> flg
any doEq2(Context *CONTEXT_PTR, any x)
{
   cell c1;

   x = cdr(x),  Push(c1, EVAL(CONTEXT_PTR, car(x)));
   while (Nil != (x = cdr(x)))
   {
      //if (data(c1) != EVAL(CONTEXT_PTR, car(x))) { // TODO CHECK IT OUT
      any x1 = car(data(c1));
      any x2 = car(EVAL(CONTEXT_PTR, car(x)));

      if (x1 && getCARType(x1) == BIN)
      {
          //if (!compareBIN(x1, x2))
          //{
          //       drop(c1);
          //       return Nil;
          //}
      }


      if ( x1 != x2)
      {
         drop(c1);
         return Nil;
      }
   }
   drop(c1);
   return T;
}

// (if 'any1 any2 . prg) -> any
any doIf(Context *CONTEXT_PTR, any x)
{
   any a;

   x = cdr(x);
   if (isNil(a = EVAL(CONTEXT_PTR, car(x))))
      return prog(CONTEXT_PTR, cddr(x));
   val(At) = a;
   x = cdr(x);
   return EVAL(CONTEXT_PTR, car(x));
}

// (de sym . any) -> sym
any doDe(Context *CONTEXT_PTR, any ex)
{
   redefine(CONTEXT_PTR, ex, cadr(ex), cddr(ex));
   return cadr(ex);
}


// (let sym 'any . prg) -> any
// (let (sym 'any ..) . prg) -> any
/*
 * Bind essentially backs up the symbols
 * For example if you have 
 * (setq X 10)
 * (let (X 20) X)
 * In this case the value 10 of X should be backed up
 * and restored after the let binding
 *
 */
any doLet(Context *CONTEXT_PTR, any x)
{
    any y;

    x = cdr(x);
    if (!isCell(y = car(x)))
    {
        bindFrame f;

        x = cdr(x),  Bind(y,f),  val(y) = EVAL(CONTEXT_PTR, car(x));
        x = prog(CONTEXT_PTR, cdr(x));
        Unbind(f);
    }
    else
    {
        // TODO check out how to do stack 
        bindFrame *f = allocFrame((length(CONTEXT_PTR, y)+1)/2);

        f->link = CONTEXT_PTR->Env.bind,  CONTEXT_PTR->Env.bind = f;
        f->i = f->cnt = 0;
        do
        {
            f->bnd[f->cnt].sym = car(y);
            f->bnd[f->cnt].val = val(car(y));
            ++f->cnt;
            val(car(y)) = EVAL(CONTEXT_PTR, cadr(y));
        }
        while (isCell(y = cddr(y)) && y != Nil);
        x = prog(CONTEXT_PTR, cdr(x));
        while (--f->cnt >= 0)
            val(f->bnd[f->cnt].sym) = f->bnd[f->cnt].val;
        CONTEXT_PTR->Env.bind = f->link;

        free(f);
    }
    return x;
}

// (bye 'num|NIL)
any doBye(Context *CONTEXT_PTR, any ex)
{
   printf("\n");
   bye(0);
   return ex;
}

void redefine(Context *CONTEXT_PTR, any ex, any s, any x)
{
   //NeedSymb(ex,s); TODO - GOTTA KNOW WHAT"S GOING ON HERE
   //CheckVar(ex,s);

   if (ex == Nil)
   {
      giveup("THIS SHOULD NOT HAPPEN");
   }

   // TODO bring back redifine message perhaps?
   //if (!isNil(val(s))  &&  s != val(s)  &&  !equal(x,val(s)))
   if (!isNil(val(s))  &&  s != val(s))
      redefMsg(CONTEXT_PTR, s,NULL);
   val(s) = x;

   setCDRType(s, PTR_CELL); // TODO - DO IT MORE NEATLY
}


void redefMsg(Context *CONTEXT_PTR, any x, any y)
{
   FILE *oSave = CONTEXT_PTR->OutFile;

   CONTEXT_PTR->OutFile = stderr;
   outString(CONTEXT_PTR, "# ");
   print(CONTEXT_PTR, x);
   if (y)
      space(CONTEXT_PTR), print(CONTEXT_PTR, y);
   outString(CONTEXT_PTR, " redefined\n");
   CONTEXT_PTR->OutFile = oSave;
}


// (line 'flg) -> lst|sym
any doLine(Context *CONTEXT_PTR, any x) {
   any y;
   int i;
   word w;
   cell c1;

   if (!CONTEXT_PTR->Chr)
      CONTEXT_PTR->Env.get(CONTEXT_PTR);
   if (eol(CONTEXT_PTR))
      return Nil;
   x = cdr(x);
   if (isNil(EVAL(CONTEXT_PTR, car(x)))) {
      Push(c1, cons(CONTEXT_PTR, mkChar(CONTEXT_PTR, CONTEXT_PTR->Chr), Nil));
      y = data(c1);
      for (;;) {
         if (CONTEXT_PTR->Env.get(CONTEXT_PTR), eol(CONTEXT_PTR))
            return Pop(c1);
         y = cdr(y) = cons(CONTEXT_PTR, mkChar(CONTEXT_PTR, CONTEXT_PTR->Chr), Nil);
      }
   }
   else {
      putByte1(CONTEXT_PTR->Chr, &i, &w, &y);
      for (;;) {
         if (CONTEXT_PTR->Env.get(CONTEXT_PTR), eol(CONTEXT_PTR))
            return popSym(CONTEXT_PTR, i, w, y, &c1);
         putByte(CONTEXT_PTR, CONTEXT_PTR->Chr, &i, &w, &y, &c1);
      }
   }
}

any doVeryLongFunc(Context *CONTEXT_PTR, any x)
{
    printf("VERY LONG FUNCTION CALLED\n");
    return x;
}

any doLongFunc(Context *CONTEXT_PTR, any x)
{
    printf("LONG FUNCTION CALLED\n");
    return x;
}

// (char) -> sym
// (char 'num) -> sym
// (char 'sym) -> num
any doChar(Context *CONTEXT_PTR, any ex) {
   any x = cdr(ex);

   if (x == Nil) {
      if (!CONTEXT_PTR->Chr)
         CONTEXT_PTR->Env.get(CONTEXT_PTR);
      x = CONTEXT_PTR->Chr<0? Nil : mkChar(CONTEXT_PTR, CONTEXT_PTR->Chr);
      CONTEXT_PTR->Env.get(CONTEXT_PTR);
      return x;
   }
   // TODO - fix this up
   // if (isNum(x = EVAL(CONTEXT_PTR, car(x)))) {
   //    int c = (int)unBox(x);

   //    ////if (c == 0)
   //    ////   return Nil;
   //    ////if (c == 127)
   //    ////   return mkChar2('^','?');
   //    ////if (c < ' ')
   //    ////   return mkChar2('^', c + 0x40);
   //    return mkChar(CONTEXT_PTR, c);
   // }
   // if (isSym(x)) {
   //    int c;

   //    if ((c = firstByte(x)) != '^')
   //       return box(c);
   //    return box((c = secondByte(x)) == '?'? 127 : c & 0x1F);
   // }
   atomError(ex,x);
}

any doIO(Context *CONTEXT_PTR, any ex) {
   any x;
   inFrame f;
   outFrame fo;

   x = cdr(ex),  x = EVAL(CONTEXT_PTR, car(x));
   rwOpen(CONTEXT_PTR, ex,x,&f, &fo);
   pushIOFiles(CONTEXT_PTR, &f, &fo);
   x = prog(CONTEXT_PTR, cddr(ex));
   popIOFiles(CONTEXT_PTR);
   return x;
}

any doIn(Context *CONTEXT_PTR, any ex) {
   any x;
   inFrame f;

   x = cdr(ex),  x = EVAL(CONTEXT_PTR, car(x));
   rdOpen(CONTEXT_PTR, ex,x,&f);
   pushInFiles(CONTEXT_PTR, &f);
   x = prog(CONTEXT_PTR, cddr(ex));
   popInFiles(CONTEXT_PTR);
   return x;
}

// (out 'any . prg) -> any
any doOut(Context *CONTEXT_PTR, any ex) {
   any x;
   outFrame f;

   x = cdr(ex),  x = EVAL(CONTEXT_PTR, car(x));
   wrOpen(CONTEXT_PTR, ex,x,&f);
   pushOutFiles(CONTEXT_PTR, &f);
   x = prog(CONTEXT_PTR, cddr(ex));
   popOutFiles(CONTEXT_PTR);
   return x;
}

// (while 'any . prg) -> any
any doWhile(Context *CONTEXT_PTR, any x) {
   any cond, a;
   cell c1;

   cond = car(x = cdr(x)),  x = cdr(x);
   Push(c1, Nil);
   while (!isNil(a = EVAL(CONTEXT_PTR, cond))) {
      val(At) = a;
      data(c1) = prog(CONTEXT_PTR, x);
   }
   return Pop(c1);
}

// (do 'flg|num ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
any doDo(Context *CONTEXT_PTR, any x)
{
    any f, y, z, a;
    word N=-1;

    x = cdr(x);
    if (isNil(f = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    if (isNum(f) && f->car < 0)
        return Nil;
    else
        N = (word)f->car;

    x = cdr(x),  z = Nil;
    for (;;)
    {
        if (N >= 0)
        {
            if (N == 0)
                return z;
            N--;
        }
        y = x;
        do
        {
            if (!isNum(z = car(y)))
            {
                if (isSym(z))
                    z = val(z);
                else if (isNil(car(z)))
                {
                    z = cdr(z);
                    if (isNil(a = EVAL(CONTEXT_PTR, car(z))))
                        return prog(CONTEXT_PTR, cdr(z));
                    val(At) = a;
                    z = Nil;
                }
                else if (car(z) == T)
                {
                    z = cdr(z);
                    if (!isNil(a = EVAL(CONTEXT_PTR, car(z))))
                    {
                        val(At) = a;
                        return prog(CONTEXT_PTR, cdr(z));
                    }
                    z = Nil;
                }
                else
                    z = evList(CONTEXT_PTR, z);
            }
        } while (Nil != (y = cdr(y)));
    }
}


void wrOpen(Context *CONTEXT_PTR, any ex, any x, outFrame *f) {
   //NeedSymb(ex,x);
   if (isNil(x))
      f->fp = stdout;
   else {
      char *nm = (char *)malloc(pathSize(CONTEXT_PTR, x));

      pathString(CONTEXT_PTR, x,nm);
      if (nm[0] == '+') {
         if (!(f->fp = fopen(nm+1, "a")))
            openErr(ex, nm);
      }
      else if (!(f->fp = fopen(nm, "w")))
         openErr(ex, nm);

      free(nm);
   }
}

void rwOpen(Context *CONTEXT_PTR, any ex, any x, inFrame *f, outFrame *fo)
{
    //NeedSymb(ex,x); // TODO WHAT IS THIS ABOUT?
    if (isNil(x))
    {
        f->fp = stdin;
    }
    else
    {
        int ps = pathSize(CONTEXT_PTR, x);
        char *nm = (char*)malloc(ps);

        pathString(CONTEXT_PTR, x,nm);
        if (nm[0] == '+')
        {
            if (!(f->fp = fo->fp = fopen(nm+1, "rw")))
            {
                openErr(ex, nm);
            }
            fseek(f->fp, 0L, SEEK_SET);
        }
        else if (!(f->fp = fo->fp = fopen(nm, "rw+")))
        {

            openErr(ex, nm);
        }

        free(nm);
    }
}

void rdOpen(Context *CONTEXT_PTR, any ex, any x, inFrame *f)
{
    //NeedSymb(ex,x); // TODO WHAT IS THIS ABOUT?
    if (isNil(x))
    {
        f->fp = stdin;
    }
    else
    {
        int ps = pathSize(CONTEXT_PTR, x);
        char *nm = (char*)malloc(ps);

        pathString(CONTEXT_PTR, x,nm);
        if (nm[0] == '+')
        {
            if (!(f->fp = fopen(nm+1, "a+")))
            {
                openErr(ex, nm);
            }
            fseek(f->fp, 0L, SEEK_SET);
        }
        else if (!(f->fp = fopen(nm, "r")))
        {
            openErr(ex, nm);
        }

        free(nm);
    }
}

void openErr(any ex, char *s)
{
    err(ex, NULL, "%s open: %s", s, strerror(errno));
}

void eofErr(void)
{
    err(NULL, NULL, "EOF Overrun");
}

void pushIOFiles(Context *CONTEXT_PTR, inFrame *f, outFrame *fo)
{
    f->next = CONTEXT_PTR->Chr,  CONTEXT_PTR->Chr = 0;
    CONTEXT_PTR->InFile = f->fp;
    f->get = CONTEXT_PTR->Env.get,  CONTEXT_PTR->Env.get = getStdin;
    f->link = CONTEXT_PTR->Env.inFrames,  CONTEXT_PTR->Env.inFrames = f;

    CONTEXT_PTR->OutFile = fo->fp;
    fo->put = CONTEXT_PTR->Env.put,  CONTEXT_PTR->Env.put = putStdout;
    fo->link = CONTEXT_PTR->Env.outFrames,  CONTEXT_PTR->Env.outFrames = fo;
}

void pushInFiles(Context *CONTEXT_PTR, inFrame *f)
{
    f->next = CONTEXT_PTR->Chr,  CONTEXT_PTR->Chr = 0;
    CONTEXT_PTR->InFile = f->fp;
    f->get = CONTEXT_PTR->Env.get,  CONTEXT_PTR->Env.get = getStdin;
    f->link = CONTEXT_PTR->Env.inFrames,  CONTEXT_PTR->Env.inFrames = f;
}

void pushOutFiles(Context *CONTEXT_PTR, outFrame *f)
{
    CONTEXT_PTR->OutFile = f->fp;
    f->put = CONTEXT_PTR->Env.put,  CONTEXT_PTR->Env.put = putStdout;
    f->link = CONTEXT_PTR->Env.outFrames,  CONTEXT_PTR->Env.outFrames = f;
}

void popIOFiles(Context *CONTEXT_PTR)
{
    if (CONTEXT_PTR->InFile != stdin)
    {
        fclose(CONTEXT_PTR->InFile);
    }

    CONTEXT_PTR->Chr = CONTEXT_PTR->Env.inFrames->next;
    CONTEXT_PTR->Env.get = CONTEXT_PTR->Env.inFrames->get;
    CONTEXT_PTR->InFile = (CONTEXT_PTR->Env.inFrames = CONTEXT_PTR->Env.inFrames->link)?  CONTEXT_PTR->Env.inFrames->fp : stdin;

    CONTEXT_PTR->Env.put = CONTEXT_PTR->Env.outFrames->put;
    CONTEXT_PTR->OutFile = (CONTEXT_PTR->Env.outFrames = CONTEXT_PTR->Env.outFrames->link)? CONTEXT_PTR->Env.outFrames->fp : stdout;
}

void popInFiles(Context *CONTEXT_PTR)
{
    if (CONTEXT_PTR->InFile != stdin)
    {
        fclose(CONTEXT_PTR->InFile);
    }
    CONTEXT_PTR->Chr = CONTEXT_PTR->Env.inFrames->next;
    CONTEXT_PTR->Env.get = CONTEXT_PTR->Env.inFrames->get;
    CONTEXT_PTR->InFile = (CONTEXT_PTR->Env.inFrames = CONTEXT_PTR->Env.inFrames->link)?  CONTEXT_PTR->Env.inFrames->fp : stdin;
}

void popOutFiles(Context *CONTEXT_PTR)
{
    if (CONTEXT_PTR->OutFile != stdout && CONTEXT_PTR->OutFile != stderr)
    {
        fclose(CONTEXT_PTR->OutFile);
    }
    CONTEXT_PTR->Env.put = CONTEXT_PTR->Env.outFrames->put;
    CONTEXT_PTR->OutFile = (CONTEXT_PTR->Env.outFrames = CONTEXT_PTR->Env.outFrames->link)? CONTEXT_PTR->Env.outFrames->fp : stdout;
}

void pathString(Context *CONTEXT_PTR, any x, char *p)
{
    int c, i;
    uword w;
    char *h;

    if ((c = getByte1(CONTEXT_PTR, &i, &w, &x)) == '+')
    {
        *p++ = c,  c = getByte(CONTEXT_PTR, &i, &w, &x);
    }
    if (c != '@')
    {
        while (*p++ = c)
        {
            c = getByte(CONTEXT_PTR,&i, &w, &x);
        }
    }
    else
    {
        if (h = CONTEXT_PTR->Home)
        {
            do
            {
                *p++ = *h++;
            }
            while (*h);
        }

        while (*p++ = getByte(CONTEXT_PTR, &i, &w, &x));
    }
}


void sym2str(Context *CONTEXT_PTR, any nm, char *buf)
{
    int i, c, ctr=0;
    word w;

    c = getByte1(CONTEXT_PTR, &i, &w, &nm);
    do
    {
        if (c == '"'  ||  c == '\\')
        {
            CONTEXT_PTR->Env.put(CONTEXT_PTR, '\\');
            buf[ctr++]=c;
        }
        CONTEXT_PTR->Env.put(CONTEXT_PTR, c);
        buf[ctr++]=c;
    }
   while (c = getByte(CONTEXT_PTR, &i, &w, &nm));
    buf[ctr++]=0;
}


any doCall(Context *CONTEXT_PTR, any ex)
{
    char buf[1024];
    any y;
    any x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    sym2str(CONTEXT_PTR, y, buf);
    system(buf);
    return x;
}

void pack(Context *CONTEXT_PTR, any x, int *i, uword *p, any *q, cell *cp)
{
   int c, j;
   word w;

   if (x != Nil && getCARType(x) == PTR_CELL && getCDRType(x) == PTR_CELL)
   {
      do
      {
         pack(CONTEXT_PTR, car(x), i, p, q, cp);
      }
      while (Nil != (x = cdr(x)));
   }
   if (isNum(x)) {
      char buf[BITS/2], *b = buf;

      bufNum(buf, unBox(x));
      do
         putByte(CONTEXT_PTR, *b++, i, p, q, cp);
      while (*b);
   }
   else if (!isNil(x))
      for (x = name(x), c = getByte1(CONTEXT_PTR, &j, &w, &x); c; c = getByte(CONTEXT_PTR,&j, &w, &x))
         putByte(CONTEXT_PTR, c, i, p, q, cp);
}

// (pack 'any ..) -> sym
any doPack(Context *CONTEXT_PTR, any x)
{
   int i;
   word w;
   any y;
   cell c1, c2;

   x = cdr(x),  Push(c1, EVAL(CONTEXT_PTR, car(x)));
   putByte0(&i, &w, &y);
   pack(CONTEXT_PTR, data(c1), &i, &w, &y, &c2);
   while (Nil != (x = cdr(x)))
   {
      pack(CONTEXT_PTR, data(c1) = EVAL(CONTEXT_PTR, car(x)), &i, &w, &y, &c2);
   }
   y = popSym(CONTEXT_PTR, i, w, y, &c2);
   drop(c1);
   return i? y : Nil;
}

any doLoad(Context *CONTEXT_PTR, any ex)
{
   any x, y;

   x = cdr(ex);
   do {
      if ((y = EVAL(CONTEXT_PTR, car(x))) != T)
         y = load(CONTEXT_PTR, ex, '>', y);
      else
         y = loadAll(CONTEXT_PTR, ex);
   } while (Nil != (x = cdr(x)));
   return y;
}

// (eval 'any ['cnt ['lst]]) -> any
any doEval(Context *CONTEXT_PTR, any x) {
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


any apply(Context *CONTEXT_PTR, any ex, any foo, bool cf, int n, cell *p) {
   while (!isFunc(foo)) {
      if (isCell(foo)) {
         int i;
         any x = car(foo);
         // struct {  // bindFrame
         //    struct bindFrame *link;
         //    int i, cnt;
         //    struct {any sym; any val;} bnd[length(CONTEXT_PTR, x)+2];
         // } f;

         bindFrame *f = allocFrame(length(CONTEXT_PTR, x) + 2);

         f->link = CONTEXT_PTR->Env.bind,  CONTEXT_PTR->Env.bind = (bindFrame*)&f;
         f->i = 0;
         f->cnt = 1,  f->bnd[0].sym = At,  f->bnd[0].val = val(At);
         while (Nil != x) {
            f->bnd[f->cnt].val = val(f->bnd[f->cnt].sym = car(x));
            val(f->bnd[f->cnt].sym) = --n<0? Nil : cf? car(data(p[f->cnt-1])) : data(p[f->cnt-1]);
            ++f->cnt, x = cdr(x);
         }
         if (isNil(x))
            x = prog(CONTEXT_PTR, cdr(foo));
         else if (x != At) {
            f->bnd[f->cnt].sym = x,  f->bnd[f->cnt].val = val(x),  val(x) = Nil;
            while (--n >= 0)
               val(x) = cons(CONTEXT_PTR, consSym(CONTEXT_PTR, cf? car(data(p[n+f->cnt-1])) : data(p[n+f->cnt-1]), 0), val(x));
            ++f->cnt;
            x = prog(CONTEXT_PTR, cdr(foo));
         }
         else {
            int cnt = n;
            int next = CONTEXT_PTR->Env.next;
            cell *arg = CONTEXT_PTR->Env.arg;
            //cell c[CONTEXT_PTR->Env.next = n];
            CONTEXT_PTR->Env.next = n;
            cell *c = (cell*)calloc(sizeof(cell), n);

            CONTEXT_PTR->Env.arg = c;
            for (i = f->cnt-1;  --n >= 0;  ++i)
               Push(c[n], cf? car(data(p[i])) : data(p[i]));
            x = prog(CONTEXT_PTR, cdr(foo));
            if (cnt)
               drop(c[cnt-1]);
            CONTEXT_PTR->Env.arg = arg,  CONTEXT_PTR->Env.next = next;
            free(c);
         }
         while (--f->cnt >= 0)
            val(f->bnd[f->cnt].sym) = f->bnd[f->cnt].val;
         CONTEXT_PTR->Env.bind = f->link;
         free(f);
         return x;
      }
//      if (val(foo) == val(Meth)) {
//         any expr, o, x;
//
//         o = cf? car(data(p[0])) : data(p[0]);
//         NeedSymb(ex,o);
//         TheCls = NULL,  TheKey = foo;
//         if (expr = method(o)) {
//            int i;
//            any cls = Env.cls, key = Env.key;
//            struct {  // bindFrame
//               struct bindFrame *link;
//               int i, cnt;
//               struct {any sym; any val;} bnd[length(x = car(expr))+3];
//            } f;
//
//            Env.cls = TheCls,  Env.key = TheKey;
//            f->link = Env.bind,  Env.bind = (bindFrame*)&f;
//            f->i = 0;
//            f->cnt = 1,  f->bnd[0].sym = At,  f->bnd[0].val = val(At);
//            --n, ++p;
//            while (isCell(x)) {
//               f->bnd[f->cnt].val = val(f->bnd[f->cnt].sym = car(x));
//               val(f->bnd[f->cnt].sym) = --n<0? Nil : cf? car(data(p[f->cnt-1])) : data(p[f->cnt-1]);
//               ++f->cnt, x = cdr(x);
//            }
//            if (isNil(x)) {
//               f->bnd[f->cnt].sym = This;
//               f->bnd[f->cnt++].val = val(This);
//               val(This) = o;
//               x = prog(cdr(expr));
//            }
//            else if (x != At) {
//               f->bnd[f->cnt].sym = x,  f->bnd[f->cnt].val = val(x),  val(x) = Nil;
//               while (--n >= 0)
//                  val(x) = cons(CONTEXT_PTR, consSym(CONTEXT_PTR, cf? car(data(p[n+f->cnt-1])) : data(p[n+f->cnt-1]), 0), val(x));
//               ++f->cnt;
//               f->bnd[f->cnt].sym = This;
//               f->bnd[f->cnt++].val = val(This);
//               val(This) = o;
//               x = prog(cdr(expr));
//            }
//            else {
//               int cnt = n;
//               int next = Env.next;
//               cell *arg = Env.arg;
//               cell c[Env.next = n];
//
//               Env.arg = c;
//               for (i = f->cnt-1;  --n >= 0;  ++i)
//                  Push(c[n], cf? car(data(p[i])) : data(p[i]));
//               f->bnd[f->cnt].sym = This;
//               f->bnd[f->cnt++].val = val(This);
//               val(This) = o;
//               x = prog(cdr(expr));
//               if (cnt)
//                  drop(c[cnt-1]);
//               Env.arg = arg,  Env.next = next;
//            }
//            while (--f->cnt >= 0)
//               val(f->bnd[f->cnt].sym) = f->bnd[f->cnt].val;
//            Env.bind = f->link;
//            Env.cls = cls,  Env.key = key;
//            return x;
//         }
//         err(ex, o, "Bad object");
//      }
      if (isNil(val(foo)) || foo == val(foo))
         undefined(foo,ex);
      foo = val(foo);
   }
   if (--n < 0)
      cdr(CONTEXT_PTR->ApplyBody) = Nil;
   else {
      any x = CONTEXT_PTR->ApplyArgs;
      val(caar(x)) = cf? car(data(p[n])) : data(p[n]);
      while (--n >= 0) {
         if (!isCell(cdr(x)))
            cdr(x) = cons(CONTEXT_PTR, cons(CONTEXT_PTR, consSym(CONTEXT_PTR, Nil,0), car(x)), Nil);
         x = cdr(x);
         val(caar(x)) = cf? car(data(p[n])) : data(p[n]);
      }
      cdr(CONTEXT_PTR->ApplyBody) = car(x);
   }
   return evSubr(foo->car, CONTEXT_PTR->ApplyBody);
}


// (mapcar 'fun 'lst ..) -> lst
any doMapcar(Context *CONTEXT_PTR, any ex) {
   any x = cdr(ex);
   cell res, foo;

   Push(res, Nil);
   Push(foo, EVAL(CONTEXT_PTR, car(x)));
   if (isCell(x = cdr(x))) {
      int i, n = 0;
      //cell c[length(CONTEXT_PTR, x)];
      cell *c = (cell *)calloc(sizeof(cell), length(CONTEXT_PTR, x));

      do
         Push(c[n], EVAL(CONTEXT_PTR, car(x))), ++n;
      while (Nil != (x = cdr(x)));
      if (!isCell(data(c[0])))
      {
          free(c);
         return Pop(res);
      }
      data(res) = x = cons(CONTEXT_PTR, apply(CONTEXT_PTR, ex, data(foo), YES, n, c), Nil);
      while (Nil != (data(c[0]) = cdr(data(c[0])))) {
         for (i = 1; i < n; ++i)
            data(c[i]) = cdr(data(c[i]));
         cdr(x) = cons(CONTEXT_PTR, apply(CONTEXT_PTR, ex, data(foo), YES, n, c), Nil);
         x = cdr(x);
      }

      free(c);
   }

   return Pop(res);
}


any doSampleOpen(Context *CONTEXT_PTR, any ex)
{
    any x1, x2;

    x1 = cdr(ex),  x1 = EVAL(CONTEXT_PTR, car(x1));
    x2 = cdr(cdr(ex)),  x2 = EVAL(CONTEXT_PTR, car(x2));


    if (x1 == Nil || x2 == Nil)
    {
        printf("BAD ARGUMENTS\n");
        return Nil;
    }


    int ps = pathSize(CONTEXT_PTR, x1);
    char *nm = (char*)malloc(ps);
    pathString(CONTEXT_PTR, x1,nm);
    ps = pathSize(CONTEXT_PTR, x1);
    char *md = (char*)malloc(ps);
    pathString(CONTEXT_PTR, x2,md);


    FILE *fp = fopen(nm, md);

//    for(int i =0; i< 10;i++)
//        mkNum(CONTEXT_PTR, i);

    // cell c;
    // Push(c, mkNum(CONTEXT_PTR, (word)w));
    // any y = Pop(c);
    // printf("---------> %p\n", y->car);


    free(nm);
    free(md);

    return mkNum(CONTEXT_PTR, (word)fp);
}

any doSampleRead(Context *CONTEXT_PTR, any ex)
{
    any x1, x2;

    x1 = cdr(ex),  x1 = EVAL(CONTEXT_PTR, car(x1));
    FILE *fp = (FILE*)(x1->car);

    char s[1024];
    fscanf(fp, "%s", s);
    printf("############### %s\n",s );
    return mkStr(CONTEXT_PTR, s);
}


any doLoop(Context *CONTEXT_PTR, any ex)
{
   any x, y, a;

   for (;;) {
      x = cdr(ex);
      do {
         if (Nil != (y = car(x))) {
            if (isNil(car(y))) {
               y = cdr(y);
               if (isNil(a = EVAL(CONTEXT_PTR, car(y))))
                  return prog(CONTEXT_PTR, cdr(y));
               val(At) = a;
            }
            else if (car(y) == T) {
               y = cdr(y);
               if (!isNil(a = EVAL(CONTEXT_PTR, car(y)))) {
                  val(At) = a;
                  return prog(CONTEXT_PTR, cdr(y));
               }
            }
            else
               evList(CONTEXT_PTR, y);
         }
      } while (Nil != (x = cdr(x)));
   }
}

