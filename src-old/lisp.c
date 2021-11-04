
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

int MEMS=420;
//any Mem[420];
any Mem;

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

// TODO - this is really a bad hack
void bufNum(char *b, word n)
{
    int i = 0, k = 0;
    char buf[BITS];

    b[0] = 0;

    if (n < 0)
    {
        b[k++] = '-';
        n*=-1;
    }

    if (n == 0)
    {
        b[0] = '0';
        b[1] = 0;
        return;
    }
    
    while (n)
    {
        int x = n % 10;
        n = n / 10;
        buf[i++]='0' + x;
    }

    for(int j = i - 1; j >= 0; j--)
    {
        b[k++]=buf[j];
        b[k]=0;
    }

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
        if (isFunc(foo = val(foo)))
        {
            foo = evSubr(foo->car,ex);
            drop(c1);
            return foo;
        }
        if (isCell(foo))
        {
            foo = evExpr(CONTEXT_PTR, foo, cdr(ex));
            drop(c1);
            return foo;
        }
    }
}

/* Evaluate a list */
any evList(Context *CONTEXT_PTR, any ex)
{
    any foo;

    if (ex == Nil) return Nil;

    if (isNum(foo = car(ex)))
        return ex;

    if (isCell(foo))
    {
        if (isFunc(foo = evList(CONTEXT_PTR, foo)))
        {
            return evSubr(foo->car,ex);
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

void initialize_context(Context *CONTEXT_PTR)
{
   heapAlloc(CONTEXT_PTR);
   CONTEXT_PTR->Intern[0] = CONTEXT_PTR->Intern[1] = CONTEXT_PTR->Transient[0] = CONTEXT_PTR->Transient[1] = Nil;

   //CONTEXT_PTR->Mem[4]=(any)CONTEXT_PTR->Mem;
   //CONTEXT_PTR->Mem[7]=(any)CONTEXT_PTR->Mem + 6;

   CONTEXT_PTR->Mem[1].cdr=(any)CONTEXT_PTR->Mem;
   CONTEXT_PTR->Mem[2].cdr=(any)&CONTEXT_PTR->Mem[1];


   for (int i = 1; i < MEMS; i++) // 2 because Nil has already been interned
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
        printf("checking... through the cells %d (typecast to int)\n", (int)CELLS); // TODO
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

void copy_mem(any M, Context *To)
{
    To->Mem=(any)calloc(sizeof(cell)*MEMS, 1);
    for(int i = 0; i < MEMS; i++)
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
            int index = (x-L)/sizeof(cell);
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
            int index = (x - L)/sizeof(cell);
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
    To->Mem=(any)calloc(1, sizeof(cell)*MEMS);

    /////////////////////////////////////////////////////
    //dumpMem(From, "DEBUG_HEAP0.txt");
    //dumpMem(To, "DEBUG_COPY0.txt");
    heap *from = From->Heaps;
    heap *to = To->Heaps;
    for(int i = 0; i < MEMS; i++)
    {
        any fromCell = &(From->Mem[i]);
        any toCell = (any)(To->Mem + i);
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
    for(int i = 0; i < MEMS; i++)
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
    for(int i = 0; i < MEMS; i++)
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
    CONTEXT_PTR->Code = cadr(x);
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



any addShortString(any m, char *s)
{
    int ctr = 0;
    int shift = 0;
    setCARType(m, TXT);
    setCDRType(m, PTR_CELL);
    for (int i = 0; *s && i < LISP_WORD_SIZE; i++)
    {
        ((*(WORD_TYPE*)m))|=(((WORD_TYPE)*s)<<shift) ;
        shift += 8;
        s++;
    }

    return m + 1;
}

any addLongString(any m, char *s)
{
    int ctr = 0;
    int shift = 0;
    setCARType(m, BIN_START);
    setCDRType(m, PTR_CELL);
    m->car = m + 1;
    m++;
    while(*s)
    {
        setCARType(m, BIN);
        setCDRType(m, PTR_CELL);

        ((*(WORD_TYPE*)m))|=(((WORD_TYPE)*s)<<shift) ;
        shift += 8;
        if (++ctr == LISP_WORD_SIZE)
        {
            ctr=0;
            shift = 0;
            if (*(s+1))
            {
                m->cdr = m + 1;
                m++;
            }
        }
        s++;
    }

    m->cdr = Mem;//TODO

    return m + 1;
}


any addString(any m, char *s)
{
    int l = strlen(s);
    if (l > LISP_WORD_SIZE)
    {
        return addLongString(m, s);
    }
    else
    {
        return addShortString(m, s);
    }
}

// add a absolute Nil
any addKeyVal1(any m, char *s)
{
    any n = addString(m, s);
    m->cdr = Mem;

    return n;

}

// add a Nil
any addKeyVal2(any m, char *s)
{
    any n = addString(m, s);
    m->cdr = Mem + 1;



    return n;

}
// add a func
any addKeyVal3(any m, char *s, void *v)
{
    any n = addString(m, s);
    m->cdr = n;

    setCARType(n, FUNC);
    setCDRType(n, PTR_CELL);
    n->car = v;
    //TODO
    n->cdr=(any)Mem;

    return n+1;

}

// add a NUM
any addKeyVal4(any m, char *s, WORD_TYPE num)
{
    any n = addString(m, s);
    m->cdr = n;

    setCARType(n, NUM);
    setCDRType(n, PTR_CELL);
    n->car = (any)num;
    // TODO
    n->cdr=(any)Mem;

    return n+1;

}


void someFun()
{
    Mem = (any)calloc(sizeof(cell), 500);

    Mem[0].car = Mem;
    Mem[0].cdr = Mem;
    Mem[0].meta.ptr = (any)0x404;


    
    any  xxx = addKeyVal1(&Mem[1], "Nil");
    xxx = addKeyVal1(xxx, "T");
    xxx = addKeyVal2(xxx, "@");
    xxx = addKeyVal2(xxx, "@@");
    xxx = addKeyVal2(xxx, "@@@");
    xxx = addKeyVal3(xxx, "quote", doQuote);
    xxx = addKeyVal3(xxx, "bye", doBye);

    xxx = addKeyVal3(xxx, "de", doDe);
    xxx = addKeyVal3(xxx, "+", doAdd);
    xxx = addKeyVal3(xxx, "-", doSub);
    xxx = addKeyVal3(xxx, "*", doMul);
    xxx = addKeyVal3(xxx, "let", doLet);
    xxx = addKeyVal3(xxx, "prinl", doPrin);
    xxx = addKeyVal3(xxx, "do", doDo);
    xxx = addKeyVal3(xxx, "setq", doSetq);
    xxx = addKeyVal3(xxx, "make", doMake);
    xxx = addKeyVal3(xxx, "link", doLink);
    xxx = addKeyVal3(xxx, "cons", doCons);
    xxx = addKeyVal3(xxx, "car", doCar);
    xxx = addKeyVal3(xxx, "cdr", doCdr);
    xxx = addKeyVal3(xxx, "dump", doDump);
    xxx = addKeyVal3(xxx, "while", doWhile);
    xxx = addKeyVal3(xxx, "in", doIn);
    xxx = addKeyVal3(xxx, "out", doOut);
    xxx = addKeyVal3(xxx, "char", doChar);
    xxx = addKeyVal3(xxx, "line", doLine);
    xxx = addKeyVal3(xxx, "not", doNot);
    xxx = addKeyVal3(xxx, "for", doFor);
    xxx = addKeyVal3(xxx, "run", doRun);
    xxx = addKeyVal3(xxx, "hs", doHS);
    xxx = addKeyVal3(xxx, "=", doEq);
    xxx = addKeyVal3(xxx, "if", doIf);
    xxx = addKeyVal3(xxx, "call", doCall);
    xxx = addKeyVal3(xxx, "pack", doPack);
    xxx = addKeyVal3(xxx, "fork", doFork);
    xxx = addKeyVal3(xxx, "sleep", doSleep);
    xxx = addKeyVal3(xxx, "io", doIO);
    xxx = addKeyVal3(xxx, "load", doLoad);
    xxx = addKeyVal3(xxx, "eval", doEval);
    xxx = addKeyVal3(xxx, "mapcar", doMapcar);
    xxx = addKeyVal3(xxx, "sampleOpen", doSampleOpen);
    xxx = addKeyVal3(xxx, "sampleRead", doSampleRead);
    xxx = addKeyVal3(xxx, "bind", doBind);
    xxx = addKeyVal3(xxx, "listen", doListen);
    xxx = addKeyVal3(xxx, "skt", doSocket);
    xxx = addKeyVal3(xxx, "connect", doConnect);
    xxx = addKeyVal3(xxx, "http", doHTTP);
    xxx = addKeyVal3(xxx, "sktClose", doSocketClose);
    xxx = addKeyVal3(xxx, "loop", doLoop);
    xxx = addKeyVal3(xxx, "chop", doChop);
    xxx = addKeyVal3(xxx, "gc", doGC);

    xxx = addKeyVal4(xxx, "Z", 10);

    xxx = addKeyVal4(xxx, "ABCDEFGHIJ", 10);
    xxx = addKeyVal3(xxx, "ABCDEFGHIJK", doLongFunc);
    xxx = addKeyVal3(xxx, "ABCDEFGHIJABCDEFGHIJK", doVeryLongFunc);

    xxx = addKeyVal3(xxx, "args", doArgs);
    xxx = addKeyVal3(xxx, "next", doNext);
    
    WORD_TYPE end = (WORD_TYPE)xxx;
    WORD_TYPE start = (WORD_TYPE)Mem;
    MEMS = (end - start)/sizeof(cell);

    fprintf(stderr, "SIZE = %d\n", MEMS);



}


int main(int ac, char *av[])
{
    someFun();

    any p = Mem;
    for(int i = 0;i < 10;i ++)
    {
        //fprintf(stderr, "SRC %p %p %p\n", p[i].car, p[i].cdr, p[i].meta.ptr);
    }

    Context *CONTEXT_PTR = &LISP_CONTEXT;
    //CONTEXT_PTR->Mem = Mem;
    copy_mem(Mem, CONTEXT_PTR);


    p = CONTEXT_PTR->Mem;
    for(int i = 0;i < 10;i ++)
    {
        //fprintf(stderr, "TGT %p %p %p\n", p[i].car, p[i].cdr, p[i].meta.ptr);
    }

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
