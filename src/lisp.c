
#include "lisp.h"

#include "cell.h"

#if INTPTR_MAX == INT32_MAX
    #include "def32.d"
#elif INTPTR_MAX == INT64_MAX
    #include "def64.d"
#else
    #error "Unsupported bit width"
#endif


bool isSym(any x)
{
   if (x) return 0;
   // TODO - this must be checked out
   return 0;
}


any evList(any);
any EVAL(any x)
{
   if (isNum(x))
   {
      return x;
   }
   else if (isTxt(x) || getCARType(x) == BIN_START)
   {
      return val(x);
   }
   else
   {
      return evList(x);
   }
}


static void gc(word c);


/* Globals */
// int Chr, Trace;
// char **AV, *AV0, *Home;
// heap *Heaps;
// cell *Avail;
// stkEnv Env;
// catchFrame *CatchPtr;
// FILE *InFile, *OutFile;
// any Intern[2], Transient[2];
// any ApplyArgs, ApplyBody;


Context CONTEXT;



///////////////////////////////////////////////
//               sym.c
///////////////////////////////////////////////


///////////////////////////////////////////////
//               sym.c - END
///////////////////////////////////////////////


///////////////////////////////////////////////
//               io.c - START
///////////////////////////////////////////////



any load(any ex, int pr, any x)
{
    cell c1, c2;
    inFrame f;

    // TODO - get back function execution from command line if (isSymb(x) && firstByte(x) == '-')

    rdOpen(ex, x, &f);
    pushInFiles(&f);
    //doHide(Nil);
    x = Nil;
    for (;;)
    {
        if (CONTEXT.InFile != stdin)
        {
            data(c1) = read1(0);
        }
        else
        {
            if (pr && !CONTEXT.Chr)
                CONTEXT.Env.put(pr), space(), fflush(CONTEXT.OutFile);
            data(c1) = read1('\n');
            while (CONTEXT.Chr > 0)
            {
                if (CONTEXT.Chr == '\n')
                {
                    CONTEXT.Chr = 0;
                    break;
                }
                if (CONTEXT.Chr == '#')
                    comment();
                else
                {
                    if (CONTEXT.Chr > ' ')
                        break;
                    CONTEXT.Env.get();
                }
            }
        }
        if (isNil(data(c1)))
        {
            popInFiles();
            doHide(Nil);
            return x;
        }
        Save(c1);
        if (CONTEXT.InFile != stdin || CONTEXT.Chr || !pr)
            // TODO - WHY @ does not work in files
            x = EVAL(data(c1));
        else
        {
            Push(c2, val(At));
            x = EVAL(data(c1));
            cdr(At) = x;
            setCDRType(At, getCDRType(x));
            //x = val(At) = EVAL(data(c1));

            cdr(At2) = c2.car;
            setCDRType(At2, getCARType(&c2));

            cdr(At3) = cdr(At2);
            setCDRType(At3, getCDRType(At2));

            //val(At3) = val(At2),  val(At2) = data(c2);
            outString("-> "),  fflush(CONTEXT.OutFile),  print(x),  newline();
        }
        drop(c1);
    }
}

/*** Prining ***/
void putStdout(int c)
{
    putc(c, CONTEXT.OutFile);
}

void newline(void)
{
    CONTEXT.Env.put('\n');
}

void space(void)
{
    CONTEXT.Env.put(' ');
}

void outString(char *s)
{
    while (*s)
        CONTEXT.Env.put(*s++);
}

int bufNum(char buf[BITS/2], word n)
{
    return sprintf(buf, WORD_FORMAT_STRING_D, n); // TODO - this is not quite right for 32 bit
}

void outNum(word n)
{
    char buf[BITS/2];

    bufNum(buf, n);
    outString(buf);
}

/* Print one expression */
void print(any x)
{
    if (x == T)
    {
        printf("T");
        return;
    }

    if (x == Nil)
    {
        printf("Nil");
        return;
    }

    if (isNum(x))
    {
        outNum(unBox(x));
        return;
    }
    if (getCARType(x) == TXT || getCARType(x) == BIN_START)
    {
        printLongTXT(x);
        return;
    }

    if (getCARType(x) == PTR_CELL && getCDRType(x) == PTR_CELL)
    {
        printf("(");
        print(x->car);
        while (x != Nil)
        {
            x = x->cdr;
            if (x->car != Nil)
            {
                printf(" ");
                print(x->car);
            }
        }
        printf(")");
        return;
    }

    printf ("TODO NOT A NUMBER %p %p\n", x, Nil);
    return;
}

void prin(any x)
{
    if (x == Nil)
    {
        printf("Nil");
        return;
    }

    if (!isNil(x))
    {
        if (isNum(x))
        {
            outNum(unBox(x));
        }
        else if (x == T)
        {
            printf("T");
        }
        else if (getCARType(x) == TXT)
        {
            printLongTXT(x);
        }
        else if (getCARType(x) == BIN_START)
        {
            printLongTXT(x);

        }
        else if (isSym(x))
        {
            int i, c;
            uword w;
            while(1);

            for (x = name(x), c = getByte1(&i, &w, &x); c; c = getByte(&i, &w, &x))
            {
                if (c != '^')
                    CONTEXT.Env.put(c);
                else if (!(c = getByte(&i, &w, &x)))
                    CONTEXT.Env.put('^');
                else if (c == '?')
                    CONTEXT.Env.put(127);
                else
                    CONTEXT.Env.put(c &= 0x1F);
            }
        }
        else
        {
            while (prin(car(x)), !isNil(x = cdr(x)))
            {
                if (!isCell(x))
                {
                    prin(x);
                    break;
                }
            }
        }
    }
}


///////////////////////////////////////////////
//               io.c - END
///////////////////////////////////////////////


///////////////////////////////////////////////
//               math.c START
///////////////////////////////////////////////



///////////////////////////////////////////////
//               math.c END
///////////////////////////////////////////////


///////////////////////////////////////////////
//               flow.c START
///////////////////////////////////////////////





///////////////////////////////////////////////
//               flow.c END
///////////////////////////////////////////////


///////////////////////////////////////////////
//               gc.c START
///////////////////////////////////////////////


///////////////////////////////////////////////
//               gc.c END
///////////////////////////////////////////////

/*** System ***/
void giveup(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

void bye(int n)
{
    exit(n);
}

/* Allocate memory */
void *alloc(void *p, size_t siz)
{
   if (!(p = realloc(p,siz)))
      giveup("No memory");
   return p;
}


/*** Error handling ***/
void err(any ex, any x, char *fmt, ...)
{
    printf("ERROR\n");
    bye(0);
    if (ex == x) bye(1);
    if (fmt == NULL) bye(1);
}

void argError(any ex, any x) {err(ex, x, "Bad argument");}
void numError(any ex, any x) {err(ex, x, "Number expected");}
void symError(any ex, any x) {err(ex, x, "Symbol expected");}
void pairError(any ex, any x) {err(ex, x, "Cons pair expected");}
void atomError(any ex, any x) {err(ex, x, "Atom expected");}
void lstError(any ex, any x) {err(ex, x, "List expected");}
void varError(any ex, any x) {err(ex, x, "Variable expected");}
void protError(any ex, any x) {err(ex, x, "Protected symbol");}

/*** Evaluation ***/
any evExpr(any expr, any x)
{
   any y = car(expr);

   bindFrame *f = allocFrame(length(y)+2);

   f->link = CONTEXT.Env.bind,  CONTEXT.Env.bind = f;
   f->i = (bindSize * (length(y)+2)) / (2*sizeof(any)) - 1;
   f->cnt = 1,  f->bnd[0].sym = At,  f->bnd[0].val = val(At);

   while (y != Nil && y != cdr(y) && 0 != cdr(y))
   {
      f->bnd[f->cnt].sym = car(y);
      f->bnd[f->cnt].val = EVAL(car(x));
      ++f->cnt;
      x = cdr(x);
      y = cdr(y);
   }

   if (isNil(y)) {
      do
      {
         x = val(f->bnd[--f->i].sym);
         val(f->bnd[f->i].sym) = f->bnd[f->i].val;
         f->bnd[f->i].val = x;
      }
      while (f->i);

      x = prog(cdr(expr));
   }
   else if (y != At)
   {
      f->bnd[f->cnt].sym = y,  f->bnd[f->cnt++].val = val(y),  val(y) = x;
      do
      {
         x = val(f->bnd[--f->i].sym);
         val(f->bnd[f->i].sym) = f->bnd[f->i].val;
         f->bnd[f->i].val = x;
      }
      while (f->i);
      x = prog(cdr(expr));
   }
   else
   {
      int n, cnt;
      cell *arg;
      cell *c = (cell*)malloc(sizeof(cell) * (n = cnt = length(x)));

      while (--n >= 0)
      {
         Push(c[n], EVAL(car(x))),  x = cdr(x);
      }

      do
      {
         x = val(f->bnd[--f->i].sym);
         val(f->bnd[f->i].sym) = f->bnd[f->i].val;
         f->bnd[f->i].val = x;
      }
      while (f->i);

      n = CONTEXT.Env.next,  CONTEXT.Env.next = cnt;
      arg = CONTEXT.Env.arg,  CONTEXT.Env.arg = c;
      x = prog(cdr(expr));
      if (cnt)
      {
         drop(c[cnt-1]);
      }

      CONTEXT.Env.arg = arg,  CONTEXT.Env.next = n;
      free(c);
   }

   while (--f->cnt >= 0)
   {
      val(f->bnd[f->cnt].sym) = f->bnd[f->cnt].val;
   }

   CONTEXT.Env.bind = f->link;
   free(f);
   return x;
}

void undefined(any x, any ex) {err(ex, x, "Undefined");}

static any evList2(any foo, any ex)
{
    cell c1;

    Push(c1, foo);
    if (isCell(foo))
    {
        foo = evExpr(foo, cdr(ex));
        drop(c1);
        return foo;
    }
    for (;;)
    {
        if (isNil(val(foo)))
            undefined(foo,ex);
        if (isNum(foo = val(foo)))
        {
            foo = evSubr(foo,ex);
            drop(c1);
            return foo;
        }
        if (isCell(foo))
        {
            foo = evExpr(foo, cdr(ex));
            drop(c1);
            return foo;
        }
    }
}

/* Evaluate a list */
any evList(any ex)
{
    any foo;

    if (ex == Nil) return Nil;

    if (isNum(foo = car(ex)))
        return ex;

    if (getCARType(ex) == BIN_START) return Nil;

    if (isCell(foo))
    {
        if (isNum(foo = evList(foo)))
            return evSubr(foo,ex);
        return evList2(foo,ex);
    }
    for (;;)
    {
        if (isNil(val(foo)))
            undefined(foo,ex);
        if (isFunc(foo))
            return evSubr(foo->cdr,ex);
        if (isNum(foo = val(foo)))
            return evSubr(foo,ex);
        if (isCell(foo))
            return evExpr(foo, cdr(ex));
    }
}

any loadAll(any ex)
{
   any x = Nil;

   while (*CONTEXT.AV  &&  strcmp(*CONTEXT.AV,"-") != 0)
      x = load(ex, 0, mkStr(*CONTEXT.AV++));
   return x;
}

void printLongTXT(any nm)
{
    int i, c;
    word w;

    c = getByte1(&i, &w, &nm);
    do
    {
        if (c == '"'  ||  c == '\\')
        {
            CONTEXT.Env.put('\\');
        }
        CONTEXT.Env.put(c);
    }
   while (c = getByte(&i, &w, &nm));
}

void printNUM(any cell)
{
    printf(WORD_FORMAT_STRING_D, (word)cell->car);
}

/*** Main ***/
int main(int ac, char *av[])
{
   if (ac == 0) printf("STRANGE\n");

   av++;
   CONTEXT.AV = av;
   heapAlloc();
   doDump(Nil);
   //getHeapSize();
   //CELLS = 1;
   CONTEXT.Intern[0] = CONTEXT.Intern[1] = CONTEXT.Transient[0] = CONTEXT.Transient[1] = Nil;

   Mem[4] = (any)Mem; // TODO - SETTING THE VALUE OF NIL
   Mem[7] = (any)(Mem+6); // TODO - SETTING THE VALUE OF NIL

   for (int i = 3; i < MEMS; i += 3) // 2 because Nil has already been interned
   {
      any cell = (any)&Mem[i];
      CellPartType carType = getCARType(cell);
      CellPartType cdrType = getCDRType(cell);

      if ((BIN_START == carType || TXT == carType) && cdrType != FUNC && cell->cdr)
      {
         intern(cell, CONTEXT.Intern);
      }
      else if ((BIN_START == carType || TXT == carType) && cdrType == FUNC && cell->cdr)
      {
         intern(cell, CONTEXT.Intern);
      }
      else if ((BIN_START == carType || TXT == carType))
      {
         intern(cell, CONTEXT.Intern);
      }
   }

   CONTEXT.InFile = stdin, CONTEXT.Env.get = getStdin;
   CONTEXT.OutFile = stdout, CONTEXT.Env.put = putStdout;
   CONTEXT.ApplyArgs = cons(cons(consSym(Nil, 0), Nil), Nil);
   CONTEXT.ApplyBody = cons(Nil, Nil);

   doDump(Nil);
   //getHeapSize();
   loadAll(NULL);
   while (!feof(stdin))
      load(NULL, ':', Nil);
   bye(0);
}
