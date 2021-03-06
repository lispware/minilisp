
#include "lisp.h"
#include "platform/platform.h"

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


any evList(Context *, any);
any EVAL(Context *CONTEXT_PTR, any x)
{
    if (isFunc(x))
    {
        // TODO - we need to fix the FUNC value perhaps
        return x;
    }
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
      return evList(CONTEXT_PTR, x);
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


Context LISP_CONTEXT;


///////////////////////////////////////////////
//               sym.c
///////////////////////////////////////////////


///////////////////////////////////////////////
//               sym.c - END
///////////////////////////////////////////////


///////////////////////////////////////////////
//               io.c - START
///////////////////////////////////////////////



any load(Context *CONTEXT_PTR, any ex, int pr, any x)
{
    cell c1, c2;
    inFrame f;

    // TODO - get back function execution from command line if (isSymb(x) && firstByte(x) == '-')

    rdOpen(CONTEXT_PTR, ex, x, &f);
    pushInFiles(CONTEXT_PTR, &f);
    //doHide(Nil);
    x = Nil;
    for (;;)
    {
        if (CONTEXT_PTR->InFile != stdin)
        {
            data(c1) = read1(CONTEXT_PTR, 0);
        }
        else
        {
            if (pr && !CONTEXT_PTR->Chr)
                CONTEXT_PTR->Env.put(CONTEXT_PTR, pr), space(CONTEXT_PTR), fflush(CONTEXT_PTR->OutFile);
            data(c1) = read1(CONTEXT_PTR, '\n');
            while (CONTEXT_PTR->Chr > 0)
            {
                if (CONTEXT_PTR->Chr == '\n')
                {
                    CONTEXT_PTR->Chr = 0;
                    break;
                }
                if (CONTEXT_PTR->Chr == '#')
                    comment(CONTEXT_PTR);
                else
                {
                    if (CONTEXT_PTR->Chr > ' ')
                        break;
                    CONTEXT_PTR->Env.get(CONTEXT_PTR);
                }
            }
        }
        if (isNil(data(c1)))
        {
            popInFiles(CONTEXT_PTR);
            doHide(CONTEXT_PTR, Nil);
            return x;
        }
        Save(c1);
        if (CONTEXT_PTR->InFile != stdin || CONTEXT_PTR->Chr || !pr)
            // TODO - WHY @ does not work in files
            x = EVAL(CONTEXT_PTR, data(c1));
        else
        {
            Push(c2, val(At));
            x = EVAL(CONTEXT_PTR, data(c1));
            cdr(At) = x;
            setCDRType(At, getCDRType(x));
            //x = val(At) = EVAL(CONTEXT_PTR, data(c1));

            cdr(At2) = c2.car;
            setCDRType(At2, getCARType(&c2));

            cdr(At3) = cdr(At2);
            setCDRType(At3, getCDRType(At2));

            //val(At3) = val(At2),  val(At2) = data(c2);
            outString(CONTEXT_PTR, "-> ");
            fflush(CONTEXT_PTR->OutFile);
            print(CONTEXT_PTR, x);
            newline(CONTEXT_PTR);

        }
        drop(c1);
    }

    return ex;
}

/*** Prining ***/
void putStdout(Context *CONTEXT_PTR, int c)
{
    putc(c, CONTEXT_PTR->OutFile);
}

void newline(Context *CONTEXT_PTR)
{
    CONTEXT_PTR->Env.put(CONTEXT_PTR, '\n');
}

void space(Context *CONTEXT_PTR)
{
    CONTEXT_PTR->Env.put(CONTEXT_PTR, ' ');
}

void outString(Context *CONTEXT_PTR, char *s)
{
    while (*s)
        CONTEXT_PTR->Env.put(CONTEXT_PTR, *s++);
}

int bufNum(char buf[BITS/2], word n)
{
    return sprintf(buf, WORD_FORMAT_STRING_D, n); // TODO - this is not quite right for 32 bit
}

void outNum(Context *CONTEXT_PTR, word n)
{
    char buf[BITS/2];

    bufNum(buf, n);
    outString(CONTEXT_PTR, buf);
}

/* Print one expression */
void print(Context *CONTEXT_PTR, any x)
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
        outNum(CONTEXT_PTR, unBox(x));
        return;
    }
    if (getCARType(x) == TXT || getCARType(x) == BIN_START)
    {
        printLongTXT(CONTEXT_PTR, x);
        return;
    }

    if (getCARType(x) == PTR_CELL && getCDRType(x) == PTR_CELL)
    {
        printf("(");
        print(CONTEXT_PTR, x->car);
        while (x != Nil)
        {
            x = x->cdr;
            if (x->car != Nil)
            {
                printf(" ");
                print(CONTEXT_PTR, x->car);
            }
        }
        printf(")");
        return;
    }

    if (getCARType(x) == FUNC)
    {
        printf ("C FUNCTION %p", x);
        return;
    }

    printf ("TODO NOT A NUMBER %p %p\n", x, Nil);
    return;
}

void prin(Context *CONTEXT_PTR, any x)
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
            outNum(CONTEXT_PTR, unBox(x));
        }
        else if (x == T)
        {
            printf("T");
        }
        else if (getCARType(x) == TXT)
        {
            printLongTXT(CONTEXT_PTR, x);
        }
        else if (getCARType(x) == BIN_START)
        {
            printLongTXT(CONTEXT_PTR, x);

        }
        else if (isSym(x))
        {
            int i, c;
            uword w;
            while(1);

            for (x = name(x), c = getByte1(CONTEXT_PTR, &i, &w, &x); c; c = getByte(CONTEXT_PTR, &i, &w, &x))
            {
                if (c != '^')
                    CONTEXT_PTR->Env.put(CONTEXT_PTR, c);
                else if (!(c = getByte(CONTEXT_PTR, &i, &w, &x)))
                    CONTEXT_PTR->Env.put(CONTEXT_PTR, '^');
                else if (c == '?')
                    CONTEXT_PTR->Env.put(CONTEXT_PTR, 127);
                else
                    CONTEXT_PTR->Env.put(CONTEXT_PTR, c &= 0x1F);
            }
        }
        else
        {
            while (prin(CONTEXT_PTR, car(x)), !isNil(x = cdr(x)))
            {
                if (!isCell(x))
                {
                    prin(CONTEXT_PTR, x);
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
any evExpr(Context *CONTEXT_PTR, any expr, any x)
{
   any y = car(expr);

   bindFrame *f = allocFrame(length(CONTEXT_PTR, y)+2);

   f->link = CONTEXT_PTR->Env.bind,  CONTEXT_PTR->Env.bind = f;
   f->i = (bindSize * (length(CONTEXT_PTR, y)+2)) / (2*sizeof(any)) - 1;
   f->cnt = 1,  f->bnd[0].sym = At,  f->bnd[0].val = val(At);

   while (y != Nil && y != cdr(y) && getCARType(y) == PTR_CELL && getCDRType(y) == PTR_CELL)
   //while (y != Nil && y != cdr(y)  && 0 != cdr(y))
   {
      f->bnd[f->cnt].sym = car(y);
      f->bnd[f->cnt].val = EVAL(CONTEXT_PTR, car(x));
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

      x = prog(CONTEXT_PTR, cdr(expr));
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
      x = prog(CONTEXT_PTR, cdr(expr));
   }
   else
   {
      int n, cnt;
      cell *arg;
      cell *c = (cell*)calloc(sizeof(cell) * (n = cnt = length(CONTEXT_PTR, x)), 1);

      while (--n >= 0)
      {
         Push(c[n], EVAL(CONTEXT_PTR, car(x))),  x = cdr(x);
      }

      do
      {
         x = val(f->bnd[--f->i].sym);
         val(f->bnd[f->i].sym) = f->bnd[f->i].val;
         f->bnd[f->i].val = x;
      }
      while (f->i);

      n = CONTEXT_PTR->Env.next,  CONTEXT_PTR->Env.next = cnt;
      arg = CONTEXT_PTR->Env.arg,  CONTEXT_PTR->Env.arg = c;
      x = prog(CONTEXT_PTR, cdr(expr));
      if (cnt)
      {
         drop(c[cnt-1]);
      }

      CONTEXT_PTR->Env.arg = arg,  CONTEXT_PTR->Env.next = n;
      free(c);
   }

   while (--f->cnt >= 0)
   {
      val(f->bnd[f->cnt].sym) = f->bnd[f->cnt].val;
   }

   CONTEXT_PTR->Env.bind = f->link;
   free(f);
   return x;
}

void undefined(any x, any ex) {err(ex, x, "Undefined");}

static any evList2(Context *CONTEXT_PTR, any foo, any ex)
{
    cell c1;

    Push(c1, foo);
    if (isCell(foo))
    {
        foo = evExpr(CONTEXT_PTR, foo, cdr(ex));
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
            foo = evExpr(CONTEXT_PTR, foo, cdr(ex));
            drop(c1);
            return foo;
        }

        // TODO - WHY THIS?
        // I had to add it for doFork to work
        return foo;
    }
}

/* Evaluate a list */
any evList(Context *CONTEXT_PTR, any ex)
{
    any foo;
    //dumpMem(CONTEXT_PTR, "DEBUG_BEFORE_evList.txt");

    if (ex == Nil) return Nil;

    if (isNum(foo = car(ex)))
        return ex;

    if (isCell(foo))
    {
        if (isFunc(foo = evList(CONTEXT_PTR, foo)))
        {
            return evSubr(foo,ex);
        }
        // TODO - this does not seem quite right - why only NUM?
        else if (isNum(foo))
        {
            return foo;
        }
        return evList2(CONTEXT_PTR, foo,ex);
    }

    for (;;)
    {
        if (isNil(val(foo)))
            undefined(foo,ex);
        if (isNum(foo = val(foo)))
            return evSubr(foo,ex);
        if (isFunc(foo))
            return evSubr(foo->car,ex);
        if (isCell(foo))
            return evExpr(CONTEXT_PTR, foo, cdr(ex));
    }
}

any loadAll(Context *CONTEXT_PTR, any ex)
{
   any x = Nil;

   while (*CONTEXT_PTR->AV  &&  strcmp(*CONTEXT_PTR->AV,"-") != 0)
      x = load(CONTEXT_PTR, ex, 0, mkStr(CONTEXT_PTR, *CONTEXT_PTR->AV++));
   return x;
}

void printLongTXT(Context *CONTEXT_PTR, any nm)
{
    int i, c;
    word w;

    c = getByte1(CONTEXT_PTR, &i, &w, &nm);
    do
    {
        if (c == '"'  ||  c == '\\')
        {
            CONTEXT_PTR->Env.put(CONTEXT_PTR, '\\');
        }
        CONTEXT_PTR->Env.put(CONTEXT_PTR, c);
    }
   while (c = getByte(CONTEXT_PTR, &i, &w, &nm));
}

void printNUM(any cell)
{
    printf(WORD_FORMAT_STRING_D, (word)cell->car);
}

int initialize_context(Context *CONTEXT_PTR)
{
   heapAlloc(CONTEXT_PTR);
   CONTEXT_PTR->Intern[0] = CONTEXT_PTR->Intern[1] = CONTEXT_PTR->Transient[0] = CONTEXT_PTR->Transient[1] = Nil;

   CONTEXT_PTR->Mem[4]=(any)CONTEXT_PTR->Mem;
   CONTEXT_PTR->Mem[7]=(any)CONTEXT_PTR->Mem + 6;

   for (int i = 3; i < MEMS; i += 3) // 2 because Nil has already been interned
   {
      any cell = (any)(CONTEXT_PTR->Mem + i);
      CellPartType carType = getCARType(cell);
      CellPartType cdrType = getCDRType(cell);

      if ((BIN_START == carType || TXT == carType))
      {
         intern(CONTEXT_PTR, cell, CONTEXT_PTR->Intern);
      }
      else if ((BIN_START == carType || TXT == carType))
      {
         intern(CONTEXT_PTR, cell, CONTEXT_PTR->Intern);
      }
      else if ((BIN_START == carType || TXT == carType))
      {
         intern(CONTEXT_PTR, cell, CONTEXT_PTR->Intern);
      }
   }
}

void *thread_func(void *arg)
{
    Context *CONTEXT_PTR = arg;

    EVAL(CONTEXT_PTR, CONTEXT_PTR->Code);
    heap *h = CONTEXT_PTR->Heaps;

    while (h)
    {
        heap *x = h;
        h = h->next;
        free(x);
    }

    free(CONTEXT_PTR->Mem);
    free(CONTEXT_PTR);

    return NULL;
}

void check(Context *CONTEXT_PTR)
{
    printf("CHECK...\n");
    heap *from = CONTEXT_PTR->Heaps;
    any Avail = CONTEXT_PTR->Avail;
    any x = Avail;

    while(from)
    {
        printf("checking... through the cells %d\n", CELLS);
        for(int j=0; j < CELLS; j++)
        {
            cell *fromCell = &(from->cells[j]);
            printf("%p %p\n", fromCell, Avail);
            if (fromCell == Avail)
            {
                printf("BOOOOM\n");
                exit(0);
            }
        }
        from=from->next;
    }
    printf("END CHECK...\n");
}

void copy_mem(any *M, Context *To)
{
    To->Mem=(any*)calloc(sizeof(cell)*MEMS, 1);
    for(int i = 0; i < MEMS; i+=3)
    {
        cell *fromCell = (any)(M + i);
        cell *toCell = (any)(To->Mem + i);

        toCell->meta.type=fromCell->meta.type;

        CellPartType carType, cdrType;
        carType = getCARType(fromCell);
        cdrType = getCDRType(fromCell);


        if (carType == UNDEFINED || carType == TXT || carType == NUM || carType == FUNC || carType == BIN)
        {
            toCell->car = fromCell->car;
        }
        else
        {
            uword x = (uword)(fromCell->car);
            uword L = (uword)(Mem);
            int index = 3*(x-L)/sizeof(cell);
            if (x)
            {
                toCell->car = (any)(To->Mem + index);
            }
            else
            {
                toCell->car = 0;
            }
        }

        if (cdrType == UNDEFINED || cdrType == TXT || cdrType == NUM || cdrType == FUNC || cdrType == BIN)
        {
            toCell->cdr = fromCell->cdr;
        }
        else
        {
            uword x = (uword)(fromCell->cdr);
            uword L = (uword)(Mem);
            int index = 3*(x - L)/sizeof(cell);
            if (x)
            {
                toCell->cdr = (any)(To->Mem + index);
            }
            else 
            {
                toCell->cdr = 0;
            }
        }
    }
}

void copy_backup_cell(cell *fromCell, cell * toCell)
{
    toCell->meta.type = fromCell->meta.type;
    fromCell->meta.ptr = toCell;
}

void copy_fixup_cell(Context *From, Context *To, cell *fromCell, cell * toCell)
{
    CellPartType carType, cdrType;
    carType = getCARType(toCell);
    cdrType = getCDRType(toCell);

    if (carType == TXT || carType == NUM || carType == FUNC || carType == BIN)
    {
        toCell->car = fromCell->car;
    }
    else
    {
        if (fromCell->car != 0)
        {
            toCell->car = fromCell->car->meta.ptr;
        }
        else
        {
            toCell->car = fromCell->car;
        }
    }

    if (cdrType == TXT || cdrType == NUM || cdrType == FUNC || cdrType == BIN)
    {
        toCell->cdr = fromCell->cdr;
    }
    else
    {
        if (fromCell->cdr != 0)
        {
            toCell->cdr = fromCell->cdr->meta.ptr;
        }
        else
        {
            toCell->cdr = fromCell->cdr;
        }
    }
}

void copy_restore_cell(Context *From, Context *To, cell *fromCell, cell *toCell)
{
    if (fromCell== From->Avail)
    {
        To->Avail = toCell;
    }
    if (fromCell== From->Intern[0])
    {
        To->Intern[0] = toCell;
    }
    if (fromCell== From->Intern[1])
    {
        To->Intern[1] = toCell;
    }
    if (fromCell== From->Transient[0])
    {
        To->Transient[0] = toCell;
    }
    if (fromCell== From->Transient[1])
    {
        To->Transient[1] = toCell;
    }
    if (fromCell== To->Code)
    {
        To->Code = toCell;
    }

    fromCell->meta.type = toCell->meta.type;
}

void copy_heap(Context *From, Context *To)
{
    for (int i = 0;i < From->HeapCount; i++)
    {
        heapAlloc(To);
    }
    To->Mem=(any*)calloc(1, sizeof(cell)*MEMS);

    /////////////////////////////////////////////////////
    //dumpMem(From, "DEBUG_HEAP0.txt");
    //dumpMem(To, "DEBUG_COPY0.txt");
    heap *from = From->Heaps;
    heap *to = To->Heaps;
    for(int i = 0; i < MEMS; i+=3)
    {
        cell *fromCell = From->Mem + i;
        cell *toCell = To->Mem + i;
        copy_backup_cell(fromCell, toCell);
    }
    while(from)
    {
        for(int j=0; j < CELLS; j++)
        {
            cell *fromCell = &from->cells[j];
            cell *toCell = &to->cells[j];
            copy_backup_cell(fromCell, toCell);
        }

        from=from->next;
        to=to->next;
    }
    //dumpMem(From, "DEBUG_HEAP1.txt");
    //dumpMem(To, "DEBUG_COPY1.txt");

    /////////////////////////////////////////////////////
    from = From->Heaps;
    to = To->Heaps;
    for(int i = 0; i < MEMS; i+=3)
    {
        cell *fromCell = From->Mem + i;
        cell *toCell = To->Mem + i;
        copy_fixup_cell(From, To, fromCell, toCell);
    }
    while(from)
    {
        for(int j=0; j < CELLS; j++)
        {
            any fromCell = &from->cells[j];
            any toCell = &to->cells[j];
            copy_fixup_cell(From, To, fromCell, toCell);

        }

        from=from->next;
        to=to->next;
    }

    /////////////////////////////////////////////////////
    from = From->Heaps;
    to = To->Heaps;
    for(int i = 0; i < MEMS; i+=3)
    {
        cell *fromCell = From->Mem + i;
        cell *toCell = To->Mem + i;
        copy_restore_cell(From, To, fromCell, toCell);
    }
    while(from)
    {
        for(int j=0; j < CELLS; j++)
        {
            any fromCell = &from->cells[j];
            any toCell = &to->cells[j];
            copy_restore_cell(From, To, fromCell, toCell);
        }

        from=from->next;
        to=to->next;
    }

    //dumpMem(From, "DEBUG_HEAP2.txt");
    //dumpMem(To, "DEBUG_COPY2.txt");
}

any doFork(Context *CONTEXT_PTR_ORIG, any x)
{
    Context *CONTEXT_PTR = CONTEXT_PTR_ORIG;
    cell c1;
    Push(c1, x);
    //dumpMem(CONTEXT_PTR_ORIG, "DEBUG_ORIGINAL.txt");
    CONTEXT_PTR = (Context*)calloc(1, sizeof(Context));


    CONTEXT_PTR->InFile = stdin, CONTEXT_PTR->Env.get = getStdin;
    CONTEXT_PTR->OutFile = stdout, CONTEXT_PTR->Env.put = putStdout;
    CONTEXT_PTR->Code = cdr(x);
    CONTEXT_PTR_ORIG->Code = CONTEXT_PTR->Code;

    CONTEXT_PTR->ApplyArgs = Nil; //cons(CONTEXT_PTR, cons(CONTEXT_PTR, consSym(CONTEXT_PTR, Nil, 0), Nil), Nil);
    CONTEXT_PTR->ApplyBody = Nil; //cons(CONTEXT_PTR, Nil, Nil);

    copy_heap(CONTEXT_PTR_ORIG, CONTEXT_PTR);
    if (!CONTEXT_PTR_ORIG->Avail) CONTEXT_PTR->Avail = 0;
    //dumpMem(CONTEXT_PTR_ORIG, "DEBUG_FROM.txt");
    //dumpMem(CONTEXT_PTR, "DEBUG_TO.txt");


    //dumpMem(CONTEXT_PTR, "DEBUG_NEW.txt");

    plt_thread_start(CONTEXT_PTR, thread_func, 0); //TODO - passing nowait seems to not work
    //EVAL(CONTEXT_PTR, CONTEXT_PTR->Code);
    //EVAL(CONTEXT_PTR_ORIG, CONTEXT_PTR_ORIG->Code);

    CONTEXT_PTR = CONTEXT_PTR_ORIG;
    return Pop(c1);
}


int main(int ac, char *av[])
{
    Context *CONTEXT_PTR = &LISP_CONTEXT;
    //CONTEXT_PTR->Mem = Mem;
    copy_mem(Mem, CONTEXT_PTR);
    initialize_context(CONTEXT_PTR);
    av++;
    CONTEXT_PTR->AV = av;

    CONTEXT_PTR->InFile = stdin, CONTEXT_PTR->Env.get = getStdin;
    CONTEXT_PTR->OutFile = stdout, CONTEXT_PTR->Env.put = putStdout;
    CONTEXT_PTR->ApplyArgs = cons(CONTEXT_PTR, cons(CONTEXT_PTR, consSym(CONTEXT_PTR, Nil, 0), Nil), Nil);
    CONTEXT_PTR->ApplyBody = cons(CONTEXT_PTR, Nil, Nil);

    //dumpMem(CONTEXT_PTR, "DEBUG_0.txt");
    //getHeapSize();
    loadAll(CONTEXT_PTR, NULL);
    while (!feof(stdin))
        load(CONTEXT_PTR, NULL, ':', Nil);
    bye(0);
}
