#include "lisp.h"
#ifndef __NET_H__
#define __NET_H__
void getStdinNet(Context *CONTEXT_PTR);
any pltBind(Context *CONTEXT_PTR, word n);
any pltConnect(Context *CONTEXT_PTR, any ex);
any pltHttp(Context *CONTEXT_PTR, any ex);
any pltListen(Context *CONTEXT_PTR, word n);
any pltSocket(Context *CONTEXT_PTR, any ex);
void popIOFilesNet(Context *CONTEXT_PTR);
void pushIOFilesNet(Context *CONTEXT_PTR, inFrame *f, outFrame *fo);
void putStdoutNet(Context *CONTEXT_PTR, int c);
any doBind(Context *CONTEXT_PTR, any ex);
any doConnect(Context *CONTEXT_PTR, any ex);
any doHTTP(Context *CONTEXT_PTR, any ex);
any doListen(Context *CONTEXT_PTR, any ex);
any doSocket(Context *CONTEXT_PTR, any ex);
any doSocketClose(Context *CONTEXT_PTR, any ex);
void releaseSocket(struct _external* obj);
char *printSocket(Context *CONTEXT_PTR, struct _external* obj);
external * copySocket(Context *CONTEXT_PTR, external *ext);
int equalSocket(Context *CONTEXT_PTR, external*x, external*y);
void pltClose(struct _external* obj);
any parse(Context *CONTEXT_PTR, any x, bool skp);
void getParse(Context *CONTEXT_PTR);
any rdList(Context *CONTEXT_PTR);
any doArgv(Context *CONTEXT_PTR, any ex);

word GetThreadID();

#define NewExternalSocket(EXT_PARAM, FD) external *EXT_PARAM = (external *)malloc(sizeof(external));\
    EXT_PARAM->type = EXT_SOCKET;\
    EXT_PARAM->release = releaseSocket;\
    EXT_PARAM->print = printSocket;\
    EXT_PARAM->equal = equalSocket;\
    EXT_PARAM->copy = copySocket;\
    EXT_PARAM->pointer = (void*)(uword)FD;

#endif
#ifndef __THREAD_H__
#define __THREAD_H__


void copyHeap(Context *From, Context *To);
void copyBackupCell(cell *fromCell, cell * toCell);
void copyFixupCell(Context *From, Context *To, cell *fromCell, cell * toCell);
void copyRestoreCell(Context *From, Context *To, cell *fromCell, cell *toCell);
void copyHeap(Context *From, Context *To);
typedef void * (*thread_func_t)(void *);
void plt_thread_start(Context *CONTEXT_PTR, thread_func_t FUNC, int wait);
void plt_sleep(int ms);
any pltGetThreadId(Context *CONTEXT_PTR);

#endif

void ppp(Context*CONTEXT_PTR, char *m, cell c);

any prog(Context *CONTEXT_PTR, any x)
{
   any y;

   do
   {
      y = EVAL(CONTEXT_PTR, car(x));
   }
   while (!isNil(x = cdr(x)));

   return y;
}

any EVAL(Context *CONTEXT_PTR, any x)
{
    if (x == T)
    {
        return T;
    }
    else if (GetType(x) == EXT)
    {
        return x;
    }
    else if (isFunc(x))
    {
        // TODO - we need to fix the FUNC value perhaps
        return x;
    }
    else if (isNum(x))
    {
        return x;
    }
    else if (isSym(x))
    {
        return val(x);
    }
    else
    {
        return evList(CONTEXT_PTR, x);
    }
}

any evExpr(Context *CONTEXT_PTR, any expr, any x)
{
   any y = car(expr);

   bindFrame *f = allocFrame(length(CONTEXT_PTR, y)+2);

   f->link = CONTEXT_PTR->Env.bind,  CONTEXT_PTR->Env.bind = f;
   f->i = (bindSize * (length(CONTEXT_PTR, y)+2)) / (2*sizeof(any)) - 1;
   f->cnt = 1,  f->bnd[0].sym = At,  f->bnd[0].val = val(At);

   while (!isNil(y) && y != cdr(y) && isCell(y))
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
         setCARType(f->bnd[f->i].sym, PTR_CELL);
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
         setCARType(f->bnd[f->i].sym, PTR_CELL);
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
         setCARType(f->bnd[f->i].sym, PTR_CELL);
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
      setCARType(f->bnd[f->cnt].sym, PTR_CELL);
   }

   CONTEXT_PTR->Env.bind = f->link;
   free(f);
   return x;
}

any evList(Context *CONTEXT_PTR, any ex)
{
    any foo;

    if (isNil(ex)) return Nil;

    if (isNum(foo = car(ex)))
        return ex;

    if (isCell(foo))
    {
        if (isFunc(foo = evList(CONTEXT_PTR, foo)))
        {
            return evSubr(car(foo), ex);
        }
        return evList2(CONTEXT_PTR, foo, ex);
    }

    for (;;)
    {
        if (isNil(val(foo)))
            return undefined(CONTEXT_PTR, foo, ex);
        if (isNum(foo = val(foo)))
            return foo;
        if (isFunc(foo))
            return evSubr(car(foo), ex);
        if (isCell(foo))
            return evExpr(CONTEXT_PTR, foo, cdr(ex));

    }
}

any evList2(Context *CONTEXT_PTR, any foo, any ex)
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
            return undefined(CONTEXT_PTR, foo, ex);

        if (isFunc(foo = val(foo)))
        {
            foo = evSubr(car(foo), ex);
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

any run(Context *CONTEXT_PTR, any x)
{
   any y;
   cell at;

   Push(at,val(At));
   do
   {
      y = EVAL(CONTEXT_PTR, car(x));
   }
   while (!isNil(x = cdr(x)));
   val(At) = Pop(at);
   return y;
}

/******************************************************************************
Helper functions to print out the symbol tree
******************************************************************************/
static void p(Context *CONTEXT_PTR, any nm)
{
    int i, c;
    uword w;

    c = getByte1(CONTEXT_PTR, &i, &w, &nm);
    do
    {
        if (c == '"'  ||  c == '\\')
        {
            printf("\\");
        }
        printf("%c", c);
    }
   while (c = getByte(CONTEXT_PTR, &i, &w, &nm));
}

static void printTab(int tab)
{
    for(int i = 0; i < tab; i++) printf("  ");
}


void printTree(Context *CONTEXT_PTR, any r, int tab, int t)
{
    printTab(tab);

    if (t == 0 ) printf("root ");
    else if (t == 1) printf("CAR ");
    else printf("CDR ");

    if (isNil(r))
    {
        printf("<Nil>\n");
        return;
    }
    else if (isSym(r))
    {
        p(CONTEXT_PTR, r);
        printf(" %p\n", r);
        return;
    }
    else
    {
        printf("%p\n", r);
    }

    printTree(CONTEXT_PTR, car(r), tab + 1, 1);
    printTree(CONTEXT_PTR, cdr(r), tab + 1, 2);
}
/******************************************************************************/

any mkStr(Context *CONTEXT_PTR, char *s)
{
   if (s && *s)
   {
      return mkSym(CONTEXT_PTR, (byte *)s);
   }
   else
   {
      return Nil;
   }
}

any isIntern(Context *CONTEXT_PTR, any nm, any tree[2])
{
    any x, y, z;
    word n;

    for (x = tree[1]; !isNil(x);)
    {
        y = car(nm);
        z = car(car(x));
        while ((n = (word)(car(y)) - (word)car(z)) == 0)
        {
            if (isNil(y)) return car(x);

            y=cdr(y);
            z=cdr(z);
        }

        x = n<0? cadr(x) : cddr(x);
    }



    return NULL;
}

int getByte(Context *CONTEXT_PTR, int *i, uword *p, any *q)
{
    int c;

    if (*i == 0)
    {
        if (!*q || isNil(*q))
        {
            return 0;
        }
        else
        {
            *i = BITS,  *p = (uword)(car(*q)),  *q = cdr(*q);
        }
    }
    c = *p & 0xff,  *p >>= 8;
    if (*i >= 8)
        *i -= 8;
    else if (isNum(*q))
    {
        *p = (uword)*q >> 2,  *q = NULL;
        c |= *p << *i;
        *p >>= 8 - *i;
        *i += BITS-9;
    }
    else
    {
        *p = (uword)tail(*q),  *q = val(*q);
        c |= *p << *i;
        *p >>= 8 - *i;
        *i += BITS-8;
    }
    c &= 0xff;

    return c;
}

any intern(Context *CONTEXT_PTR, any sym, any tree[2])
{
   any nm, x;
   word n;

   return internBin(CONTEXT_PTR, sym, tree);

}

void putByte(Context *CONTEXT_PTR, int ch, int *bitCount, uword *acc, any *curCell, cell *prevCell)
{
    ch = ch & 0xff;
    int d = 8;

    if (*bitCount != BITS)
        *acc |= (uword)ch << *bitCount;

    if (*bitCount + d  > BITS)
    {
        if (*curCell)
        {
            any x = consName(CONTEXT_PTR, *acc, Nil);
            setCARType(x, BIN);
            cdr(*curCell) = x;
            setCARType(*curCell, BIN);
            *curCell = x;
        }
        else
        {
            any x = consSym(CONTEXT_PTR, NULL, Nil);
            setCARType(x, BIN_START);
            Push(*prevCell, x);
            any y = consName(CONTEXT_PTR, *acc, Nil);
            setCARType(y, BIN);
            *curCell = y;
            setCARType(*curCell, BIN);
            car(car(prevCell)) = *curCell;
        }
        *acc = ch >> BITS - *bitCount;
        *bitCount -= BITS;
    }

    *bitCount += d;
}

void putByte0(int *i, uword *p, any *q)
{
    *p = 0;
    *i = 0;
    *q = NULL;
}

any internBin(Context *CONTEXT_PTR, any sym, any tree[2])
{
    any nm, x, y, z;
    word n;

    x = tree[1];

    if (isNil(x))
    {
        tree[1] = consIntern(CONTEXT_PTR, sym, Nil);
        return tree[1];
    }

    for (;;)
    {

        y = car(sym);
        z = car(car(x));
        while ((n = (word)(car(y)) - (word)car(z)) == 0)
        {
            if (GetType(y) != BIN) return sym;
            y=cdr(y);
            z=cdr(z);
        }

        if (isNil(cdr(x)))
        {
            if (n < 0)
            {
                any xx = consIntern(CONTEXT_PTR, sym, Nil);
                setCARType(xx, PTR_CELL);
                xx = consIntern(CONTEXT_PTR, xx, Nil);
                setCARType(xx, PTR_CELL);
                cdr(x) = xx;
                setCARType(x, PTR_CELL);
                return sym;
            }
            else
            {
                any xx = consIntern(CONTEXT_PTR, sym, Nil);
                setCARType(xx, PTR_CELL);
                xx = consIntern(CONTEXT_PTR, Nil, xx);
                setCARType(xx, PTR_CELL);
                cdr(x) = xx;
                setCARType(x, PTR_CELL);
                return sym;
            }

        }
        if (n < 0)
        {
            if (!isNil(cadr(x)))
            {
                x = cadr(x);
            }
            else
            {
                cadr(x) = consIntern(CONTEXT_PTR, sym, Nil);
                setCARType(car(x), PTR_CELL);
                return sym;
            }
        }
        else
        {
            if (!isNil(cddr(x)))
            {
                x = cddr(x);
            }
            else
            {
                cddr(x) = consIntern(CONTEXT_PTR, sym, Nil);
                setCARType(cdr(x), PTR_CELL);
                return sym;
            }
        }
    }
}

int getByte1(Context *CONTEXT_PTR, int *i, uword *p, any *q)
{
    int c;

    if (isSym(*q))
    {
        (*q)=car(*q);
        *i = BITS, *p = (uword)(car(*q)) , *q = (cdr(*q));
    }
    else
    {
        giveup("Cant getByte");
    }

    c = *p & 0xff, *p >>= 8, *i -= 8;

    return c;
}

int symBytes(Context *CONTEXT_PTR, any x)
{
    int cnt = 0;
    uword w;

    if (isNil(x))
        return 0;

    if (isSym(x))
    {

        x = car(x);
        while (!isNil(x))
        {
			w = (uword)(car(x));
            while (w)
            {
                ++cnt;
                w >>= 8;
            }
            x = cdr(x);
        }
    }

    return cnt;
}

void putByte1(int c, int *i, uword *p, any *q)
{
    *p = c & 0xff;
    *i = 8;
    *q = NULL;
}

any symToNum(Context *CONTEXT_PTR, any sym, int scl, int sep, int ign)
{
    unsigned c;
    int i;
    uword w;
    bool sign, frac;
    any s = sym;
    int base = 10;



    if (!(c = getByte1(CONTEXT_PTR, &i, &w, &s)))
    {
        return NULL;
    }

    while (c <= ' ')  /* Skip white space */
    {
        if (!(c = getByte(CONTEXT_PTR, &i, &w, &s)))
        {
            return NULL;
        }
    }

    int LEN = pathSize(CONTEXT_PTR, sym);
    int CTR = 0;
    char *str = (char *)calloc(LEN, 1);
    
    sign = NO;
    if (c == '+'  ||  c == '-' && (sign = YES))
    {
        str[CTR++] = c;
        if (!(c = getByte(CONTEXT_PTR, &i, &w, &s)))
        {
            goto returnNULL;
        }
    }
    str[CTR++] = c;

    if ((c -= '0') > 9)
    {
        goto returnNULL;
    }

    if (c == 0)
    {
        c = getByte(CONTEXT_PTR, &i, &w, &s);
        if (c == 'x' || c == 'X') base = 16;
        else str[CTR++] = c;
    }


    while (c = getByte(CONTEXT_PTR, &i, &w, &s))
    {
        if ((int)c != ign)
        {
            str[CTR++] = c;
            if ((c -= '0') > 9)
            {
                goto returnNULL;
            }
        }
    }

    if (c)
    {
        if (c == 'H' || c == 'h') base = 16;
        else if (c == 'B' || c == 'b') base = 2;
        else if ((c -= '0') > 9) goto returnNULL;
    }


    MP_INT *BIGNUM = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init_set_str(BIGNUM, str, base);
    free(str);
    NewNumber(BIGNUM, r);
    return r;

returnNULL:
    free(str);
    return NULL;

}

any mkChar(Context *CONTEXT_PTR, int c)
{
    cell c1;
    any r = cons(CONTEXT_PTR, Nil, Nil);
    Push (c1, r);
    car(data(c1)) = cons(CONTEXT_PTR, Nil, Nil);
    cdr(data(c1)) = Nil;
    car(car(data(c1))) = (any)(uword)c;
    car(cdr(data(c1))) = Nil;

    setCARType(data(c1), BIN_START);
    setCARType(car(data(c1)), BIN);

    return Pop(c1);
}

any mkSym(Context *CONTEXT_PTR, byte *s)
{
    int i;
    uword w;
    cell c1, *p;

    putByte1(*s++, &i, &w, &p);
    while (*s)
    {
        putByte(CONTEXT_PTR, *s++, &i, &w, &p, &c1);
    }
    return popSym(CONTEXT_PTR, i, w, p, &c1);
}

any popSym(Context *CONTEXT_PTR, int i, uword n, any q, cell *cp)
{
    if (q)
    {
        cdr(q) = consName(CONTEXT_PTR, n, Nil);
        setCARType(q, BIN);
        setCARType(cdr(q), BIN);
        return Pop(*cp);
    }
    else
    {
        any x = consSym(CONTEXT_PTR, NULL, Nil);
        setCARType(x, BIN_START);
        Push(*cp, x);
        any y = consName(CONTEXT_PTR, n, Nil);
        setCARType(y, BIN);
        car(car(cp)) = y;
        return Pop(*cp);
    }
}

int firstByte(Context*CONTEXT_PTR, any s)
{
    if (isSym(s))
    {
        return ((uword)(car(car(s)))) & 0xff;
    }
    else
    {
        giveup("Cant get first byte");
        return -1;
    }
}

static int INDEX;

extern int CONSCTR;

char *ExtTypeString(any cell, char*buf)
{
    external *e = (external *)car(cell);
    int len;
    mp_err _mp_error;

    if (!e) return "NULL";
    switch(e->type)
    {
        case EXT_NUM:
            _mp_error = mp_radix_size((mp_int*)car(cell), 10, &len);
            _mp_error = mp_to_radix(num(cell), buf, len, NULL, 10);
            return "EXT_NUM";
        case EXT_SOCKET: return "EXT_SOCKET";
        default: return "UNKNOWN";
    }
}

char *TypeString(any cell, char *buf)
{
    int type = GetType(cell);
    switch(type)
    {
        case FUNC: return "FUNC";
        case PTR_CELL: return "PTR_CELL";
        case BIN: return "BIN";
        case EXT: return ExtTypeString(cell, buf);
        default: return "UNKNOWN";
    }
}

void printCell(FILE *fp, any cell)
{
    if (BIN == GetType(cell))
    {
        char buf[100];
        int bufctr=0;
        char *ptr = (char*)cell;
        for(int i = 0 ; i < BITS; i+=8)
        {
            if (*ptr) buf[bufctr++]=*ptr;
            ptr++;
        }
        buf[bufctr++] = 0;

        fprintf(fp, "%018p %018p %018p [%c] %s %s\n", &cell->car, cell->car, cell->cdr, cell->mark?'m':' ', TypeString(cell, NULL), buf);
    }
    else
    {
        char numbuf[100];
        numbuf[0]=0;
        char *p = TypeString(cell, numbuf);
        fprintf(fp, "%018p %018p %018p [%c] %s %s\n", &cell->car, cell->car, cell->cdr, cell->mark?'m':' ', p, numbuf);
    }
}

static void dumpHeap(heap *h, FILE *fp)
{
    if(!h) return;

    dumpHeap(h->next, fp);
    fprintf(fp, "HEAP\n");
    for(int i=0; i < CELLS; i++)
    {
        any c = &(h->cells[i]);
        printCell(fp, c);
    }
}

void dumpStack(Context *CONTEXT_PTR, FILE *fp)
{
    any stackptr = CONTEXT_PTR->Env.stack;

    fprintf(fp, "STACK\n");
    while (stackptr)
    {
        any cell = car(stackptr);
        printCell(fp, cell);
        stackptr=cdr(stackptr);
    }
}

void dumpMemory(Context *CONTEXT_PTR, char *name)
{
#ifdef DEBUG
    char fileName[40];
    sprintf(fileName, "%05d_%s_%d.dump",INDEX++, name, CONTEXT_PTR->THREAD_COUNT);
    FILE *fp = fopen(fileName, "w");
    fprintf(fp, "MEM %018p\n", CONTEXT_PTR->Avail);

    for (int i = 0; i < MEMS; i++)
    {
        any cell = (any)(CONTEXT_PTR->Mem + i);
        printCell(fp, cell);
    }

    fprintf(fp, "---------------------------\n");

    dumpHeap(CONTEXT_PTR->Heaps, fp);

    dumpStack(CONTEXT_PTR, fp);

    fclose(fp);
#endif
}

#define setMark(cell, m) makeptr(cell)->mark = m
#define getMark(cell) (makeptr(cell)->mark)

void markAll(Context *CONTEXT_PTR)
{
   any p;
   int i;

   for (i = 0; i < MEMS; i ++)
   {
       setMark((any)(CONTEXT_PTR->Mem + i), 0);
       mark(CONTEXT_PTR, (any)(CONTEXT_PTR->Mem + i));
   }

   /* Mark */
   setMark(CONTEXT_PTR->Intern[0], 0);mark(CONTEXT_PTR, CONTEXT_PTR->Intern[0]);
   setMark(CONTEXT_PTR->Intern[1], 0);mark(CONTEXT_PTR, CONTEXT_PTR->Intern[1]);
   setMark(CONTEXT_PTR->Transient[0], 0);mark(CONTEXT_PTR, CONTEXT_PTR->Transient[0]);
   setMark(CONTEXT_PTR->Transient[1], 0);mark(CONTEXT_PTR, CONTEXT_PTR->Transient[1]);
   if (CONTEXT_PTR->ApplyArgs) setMark(CONTEXT_PTR->ApplyArgs, 0);mark(CONTEXT_PTR, CONTEXT_PTR->ApplyArgs);
   if (CONTEXT_PTR->ApplyBody) setMark(CONTEXT_PTR->ApplyBody, 0);mark(CONTEXT_PTR, CONTEXT_PTR->ApplyBody);
   for (p = CONTEXT_PTR->Env.stack; p; p = cdr(p))
   {
      mark(CONTEXT_PTR, car(p));
   }
   for (p = (any)CONTEXT_PTR->Env.bind;  p;  p = (any)((bindFrame*)p)->link)
   {
      for (i = ((bindFrame*)p)->cnt;  --i >= 0;)
      {
         mark(CONTEXT_PTR, ((bindFrame*)p)->bnd[i].sym);
         mark(CONTEXT_PTR, ((bindFrame*)p)->bnd[i].val);
      }
   }
}

static int CTR;

void gc(Context *CONTEXT_PTR, word c)
{
    any p;
    heap *h;

    //fprintf(stderr, "GC CALLED %p\n", pthread_self());

    CTR++;
    dump("gc1");
    markAll(CONTEXT_PTR);
    dump("gc2");

    /* Sweep */
    for (int i = 0; i < MEMS; i ++)
    {
        setMark((any)(CONTEXT_PTR->Mem + i), 0);
    }
    CONTEXT_PTR->Avail = NULL;
    h = CONTEXT_PTR->Heaps;
    if (c)
    {
        do
        {
            p = h->cells + CELLS-1;
            do
            {
                if (!getMark(p))
                {
#ifdef DEBUG
                    FILE *fp = fopen("freemem.dump", "a");
                    printCell(fp, p);
                    fclose(fp);
#endif
                    if (GetType(p) == EXT)
                    {
                        external *e = (external*)car(p);
                        if (e) e->release(e);
                    }
                    memset(p, 0, sizeof(cell));
                    car(p) = CONTEXT_PTR->Avail;
                    CONTEXT_PTR->Avail = p;
                    --c;
                }
                setMark(p, 0);
            }
            while (--p >= h->cells);
        } while (h = h->next);


        while (c >= 0)
        {
            heapAlloc(CONTEXT_PTR),  c -= CELLS;
        }
    }

    dump("gc3");
    return;
}

void heapAlloc(Context *CONTEXT_PTR)
{
   heap *h;
   cell *p;

   CONTEXT_PTR->HeapCount++;

   h = (heap*)((word)calloc(1, sizeof(heap) + sizeof(cell)));
   h->next = CONTEXT_PTR->Heaps,  CONTEXT_PTR->Heaps = h;
   p = h->cells + CELLS-1;
   do
   {
      //Free(p);
      car(p) = CONTEXT_PTR->Avail;
      CONTEXT_PTR->Avail = p;
   }
   while (--p >= h->cells);
}

void *allignedAlloc(size_t size)
{
    char *p = (char*)calloc(size + 8 + sizeof(void*), 1);
    char *q = p;
    for(int i = 0;i < 8; i++)
    {
        uword w = (uword)p;
        if (!(w & 0x7))
        {
            q = p;
        }
        if (w & 0x7) p++;
    }

    return q;
}

void mark(Context *CONTEXT_PTR, any x)
{
    if (!x) return;

    if (getMark(x)) return;

    setMark(x, 1);

    if (isNil(x)) return;

    if (isSym(x))
    {
        mark(CONTEXT_PTR, cdr(x));

        x = car(x);
        while(!isNil(x))
        {
            setMark(x, 1);
            x=cdr(x);
        }
        return;
    }

    if (isCell(x))
    {
        mark(CONTEXT_PTR, car(x));
        x = cdr(x);
        do
        {
            mark(CONTEXT_PTR, car(x));
            setMark(x, 1);
            x = cdr(x);
        } while (!getMark(x));
    }
}

/* Allocate memory */
void *alloc(void *p, size_t siz)
{
   if (!(p = realloc(p,siz)))
      giveup("No memory");
   return p;
}


void giveup(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

void bye(int n)
{
    exit(n);
}

int eqList(Context *CONTEXT_PTR, any v1, any v2)
{
    while(!isNil(v1))
    {
        if (isNil(v2)) return 1;

        CellPartType t1 = GetType(v1);
        CellPartType t2 = GetType(v2);

        if (t1 != t2) return -1;

        if (isCell(v1) && isCell(car(v1)))
        {
            int r = eqList(CONTEXT_PTR, car(v1), car(v2));
            if (0 != r) return r;
        }
        else if (!isCell(v1))
        {
            int r = equal(CONTEXT_PTR, v1, v2);
            if (0 != r) return r;
        }
        else
        {
            int r = equal(CONTEXT_PTR, car(v1), car(v2));
            if (0 != r) return r;
        }

        v1 = cdr(v1);
        v2 = cdr(v2);
    }

    if (!isNil(v1)) return -1;

    if (!isNil(v2)) return 1;

    return 0;
}

// (and 'any ..) -> any
any doAnd(Context *CONTEXT_PTR, any x) {
   any a;

   x = cdr(x);
   do
   {
      if (isNil(a = EVAL(CONTEXT_PTR, car(x))))
         return Nil;
      val(At) = a;
   } while (!isNil(x = cdr(x)));
   return a;
}

// (or 'any ..) -> any
any doOr(Context *CONTEXT_PTR, any x) {
   any a;

   x = cdr(x);
   do
   {
      if (!isNil(a = EVAL(CONTEXT_PTR, car(x))))
      {
         return val(At) = a;
      }
   } while (!isNil(x = cdr(x)));

   return Nil;
}

any doNot(Context *CONTEXT_PTR, any x)
{
   any a;

   if (isNil(a = EVAL(CONTEXT_PTR, cadr(x))))
      return T;
   val(At) = a;
   return Nil;
}

// (eval 'any ['cnt ['lst]]) -> any
any doEval(Context *CONTEXT_PTR, any x)
{
   any y;
   cell c1;
   bindFrame *p;


   x = cdr(x),  Push(c1, EVAL(CONTEXT_PTR, car(x))),  x = cdr(x);
   if (!isNum(y = EVAL(CONTEXT_PTR, car(x))) || !(p = CONTEXT_PTR->Env.bind))
      data(c1) = EVAL(CONTEXT_PTR, data(c1));

   return Pop(c1);
}

int equal(Context *CONTEXT_PTR, any v, any v2)
{
    CellPartType vt = GetType(v);
    CellPartType t = GetType(v2);

    if (t != vt)
    {
        return 1;
    }

    if (t == EXT)
    {
        external *e1 = (external*)car(v);
        external *e2 = (external*)car(v2);
        return e1->equal(CONTEXT_PTR, e1, e2);
    }
    else if (isSym(v2))
    {
        int i, j;
        word w1, w2;

        word c = getByte1(CONTEXT_PTR, &i, &w1, &v);
        word d = getByte1(CONTEXT_PTR, &j, &w2, &v2);

        if (c == d)
        {
            while (c == d)
            {
                if (c == 0) return 0;
                c = getByte(CONTEXT_PTR, &i, &w1, &v);
                d = getByte(CONTEXT_PTR, &j, &w2, &v2);
            }
        }

        return (c - d) < 0 ? -1 : 1;
    }
    else if (isCell(v2))
    {
        return eqList(CONTEXT_PTR, v, v2);
    }
    else
    {
        if ( v != v2)
        {
            return 1;
        }
    }

    return 0;
}


// (= 'any ..) -> flg
any doEq(Context *CONTEXT_PTR, any x)
{
    cell c1;

    x = cdr(x),  Push(c1, EVAL(CONTEXT_PTR, car(x)));
    x = cdr(x);
    while (!isNil(x))
    {
        if (0 != equal(CONTEXT_PTR, data(c1), EVAL(CONTEXT_PTR, car(x))))
        {
            drop(c1);
            return Nil;
        }

        x = cdr(x);
    }

    drop(c1);
    return T;
}

// (cmp 'any ..) -> flg
any doCmp(Context *CONTEXT_PTR, any x)
{
    mp_err _mp_error;
    cell c1;

    x = cdr(x),  Push(c1, EVAL(CONTEXT_PTR, car(x)));
    x = cdr(x);
    while (!isNil(x))
    {
        int r = equal(CONTEXT_PTR, data(c1), EVAL(CONTEXT_PTR, car(x)));
        if (0 != r)
        {
            drop(c1);

            MP_INT *id = (MP_INT*)malloc(sizeof(MP_INT));
            mpz_init(id);
            mpz_set_si(id, r);
            NewNumber( id, idr);
            return idr;
        }

        x = cdr(x);
    }

    drop(c1);

    MP_INT *id = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(id);
    mpz_set_ui(id, 0);

    NewNumber( id, idr);
    return idr;
}

void sym2str(Context *CONTEXT_PTR, any nm, char *buf)
{
    int i, c, ctr=0;
    uword w;

    c = getByte1(CONTEXT_PTR, &i, &w, &nm);
    do
    {
        buf[ctr++]=c;
    }
    while (c = getByte(CONTEXT_PTR, &i, &w, &nm));
    buf[ctr++]=0;
}


any doRun(Context *CONTEXT_PTR, any x)
{
   any y;
   cell c1;
   bindFrame *p;

   x = cdr(x),  data(c1) = EVAL(CONTEXT_PTR, car(x)),  x = cdr(x);
   if (!isNum(data(c1)))
   {
      Save(c1);
      if (!isNum(y = EVAL(CONTEXT_PTR, car(x))) || !(p = CONTEXT_PTR->Env.bind))
         data(c1) = run(CONTEXT_PTR, data(c1));
   }
   return Pop(c1);
}

any doArgv(Context *CONTEXT_PTR, any ex)
{
    any x, y;
    char **p;
    cell c1;

    if (*(p = CONTEXT_PTR->AV) && strcmp(*p,"-") == 0)
        ++p;
    if (isNil(x = cdr(ex)))
    {
        if (!*p)
        {
            return Nil;
        }
        Push(c1, x = cons(CONTEXT_PTR, mkStr(CONTEXT_PTR, *p++), Nil));
        while (*p)
        {
            x = cdr(x) = cons(CONTEXT_PTR, mkStr(CONTEXT_PTR, *p++), Nil);
        }
        return Pop(c1);
    }
    do
    {
        if (!isCell(x))
        {
            if (!*p)
            {
                return val(x) = Nil;
            }
            Push(c1, y = cons(CONTEXT_PTR, mkStr(CONTEXT_PTR, *p++), Nil));
            while (*p)
            {
                y = cdr(y) = cons(CONTEXT_PTR, mkStr(CONTEXT_PTR, *p++), Nil);
            }
            return val(x) = Pop(c1);
        }
        y = car(x);
        val(y) = *p? mkStr(CONTEXT_PTR, *p++) : Nil;
    } while (!isNil(x = cdr(x)));
    return val(y);
}

// (args) -> flg
any doArgs(Context *CONTEXT_PTR, any ex)
{
   return CONTEXT_PTR->Env.next > 0? T : Nil;
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

// (if 'any1 any2 . prg) -> any
any doCond(Context *CONTEXT_PTR, any x)
{
   any a;

   while (!isNil(x = cdr(x)))
   {
      if (!isNil(a = EVAL(CONTEXT_PTR, caar(x))))
      {
         val(At) = a;
         return prog(CONTEXT_PTR, cdar(x));
      }
   }
   return Nil;
}

// (next) -> any
any doNext(Context *CONTEXT_PTR, any ex)
{
   if (CONTEXT_PTR->Env.next > 0)
      return data(CONTEXT_PTR->Env.arg[--CONTEXT_PTR->Env.next]);
   if (CONTEXT_PTR->Env.next == 0)
      CONTEXT_PTR->Env.next = -1;
   return Nil;
}

any doHide(Context* CONTEXT_PTR, any ex)
{
   // TODO - is this needed?
   // printf("%p\n", ex);

   return Nil;
}


// (pack 'any ..) -> sym
any doPack(Context *CONTEXT_PTR, any x)
{
   int i;
   uword w;
   any y;
   cell c1, c2;

   x = cdr(x),  Push(c1, EVAL(CONTEXT_PTR, car(x)));
   putByte0(&i, &w, &y);
   pack(CONTEXT_PTR, data(c1), &i, &w, &y, &c2);
   while (!isNil(x = cdr(x)))
   {
      pack(CONTEXT_PTR, data(c1) = EVAL(CONTEXT_PTR, car(x)), &i, &w, &y, &c2);
   }
   y = popSym(CONTEXT_PTR, i, w, y, &c2);
   drop(c1);

   return i? y : Nil;
}

// (chop 'any) -> lst
any doChop(Context *CONTEXT_PTR, any x)
{
    int c, i;
    uword w;
    char *h;
    cell c1, c2;
    any y = Nil;

    x = cdr(x);
    x = EVAL(CONTEXT_PTR, car(x));

    if(isNil(x)) return Nil;

    c = getByte1(CONTEXT_PTR, &i, &w, &x);
    Push(c1, cons(CONTEXT_PTR, mkChar(CONTEXT_PTR, c), Nil));
    y = data(c1);
    while (c)
    {
        c = getByte(CONTEXT_PTR,&i, &w, &x);
        if (c)
        {
            cdr(y) = cons(CONTEXT_PTR, mkChar(CONTEXT_PTR, c), Nil);
            setCARType(y, PTR_CELL);
            y = cdr(y);
        }
    }

    return Pop(c1);

}

void pack(Context *CONTEXT_PTR, any x, int *i, uword *p, any *q, cell *cp)
{
    int c, j;
    uword w;

    if (!isNil(x) && isCell(x))
    {
        do
        {
            if (GetType(x) == PTR_CELL)
            {
                pack(CONTEXT_PTR, car(x), i, p, q, cp);
            }
            else
            {
                pack(CONTEXT_PTR, x, i, p, q, cp);
            }
        }
        while (!isNil(x = cdr(x)));
    }
    if (isNum(x))
    {
        char *buf = mpz_get_str(NULL, 10, num(x));
        char *b = buf;

        do
        {
            putByte(CONTEXT_PTR, *b++, i, p, q, cp);
        }
        while (*b);
        free(buf);
    }
    else if (!isNil(x))
    {
        for (x = x, c = getByte1(CONTEXT_PTR, &j, &w, &x); c; c = getByte(CONTEXT_PTR,&j, &w, &x))
        {
            putByte(CONTEXT_PTR, c, i, p, q, cp);
        }
    }
}

any doDo(Context *CONTEXT_PTR, any x)
{
    any f, y, z, a;
    MP_INT CTR, ONE;
    mpz_init(&ONE);
    mpz_init(&CTR);

    mpz_set_ui(&ONE, 1);

    x = cdr(x);
    if (isNil(f = EVAL(CONTEXT_PTR, car(x))))
    {
        mpz_clear(&CTR);
        mpz_clear(&ONE);
        return Nil;
    }
    else
    {
        mpz_set(&CTR, num(f));
    }

    x = cdr(x),  z = Nil;
    for (;;)
    {
        int cmp = mpz_cmp(&CTR, &ONE); 
        if (cmp >= 0)
        {
            mpz_sub_ui(&CTR, &CTR, 1);
        }
        else
        {
            mpz_clear(&CTR);
            mpz_clear(&ONE);
            return z;
        }
        y = x;
        do
        {
            if (!isNum(z = car(y)))
            {
                if (isNil(car(z)))
                {
                    z = cdr(z);
                    if (isNil(a = EVAL(CONTEXT_PTR, car(z))))
                    {
                        mpz_clear(&CTR);
                        mpz_clear(&ONE);
                        return prog(CONTEXT_PTR, cdr(z));
                    }
                    val(At) = a;
                    z = Nil;
                }
                else if (car(z) == T)
                {
                    z = cdr(z);
                    if (!isNil(a = EVAL(CONTEXT_PTR, car(z))))
                    {
                        val(At) = a;
                        mpz_clear(&CTR);
                        mpz_clear(&ONE);
                        return prog(CONTEXT_PTR, cdr(z));
                    }
                    z = Nil;
                }
                else
                {
                    z = evList(CONTEXT_PTR, z);
                }
            }
        } while (!isNil(y = cdr(y)));
    }
}

// (quote . any) -> any
any doQuote(Context *CONTEXT_PTR, any x)
{
    return cdr(x);
}


any mkNum(Context *CONTEXT_PTR, word n)
{
    MP_INT *BIGNUM = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(BIGNUM);
    mpz_set(BIGNUM, n);
    NewNumber( BIGNUM, r);
    return r;
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
    y = car(x);
    if (!isCell(y))
    {
        bindFrame f;

        x = cdr(x);
        Bind(y,f);
        val(y) = EVAL(CONTEXT_PTR, car(x));
        setCARType(y, PTR_CELL);
        x = prog(CONTEXT_PTR, cdr(x));
        Unbind(f);
    }
    else
    {
        // TODO check out how to do stack 
        bindFrame *f = allocFrame((length(CONTEXT_PTR, y)+1)/2);

        f->link = CONTEXT_PTR->Env.bind;
        CONTEXT_PTR->Env.bind = f;
        f->i = f->cnt = 0;
        do
        {
            f->bnd[f->cnt].sym = car(y);
            f->bnd[f->cnt].val = val(car(y));
            ++f->cnt;
            val(car(y)) = EVAL(CONTEXT_PTR, cadr(y));
            setCARType(car(y), PTR_CELL);

            y = cddr(y);
        }
        while (isCell(y) && !isNil(y));
        x = prog(CONTEXT_PTR, cdr(x));
        while (--f->cnt >= 0)
            val(f->bnd[f->cnt].sym) = f->bnd[f->cnt].val;
        CONTEXT_PTR->Env.bind = f->link;

        free(f);
    }
    return x;
}

// (de sym . any) -> sym
any doDe(Context *CONTEXT_PTR, any ex)
{
    any s = cadr(ex);
    val(s) = cddr(ex);
    return s;
}


// (for sym 'num ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
// (for sym|(sym2 . sym) 'lst ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
// (for (sym|(sym2 . sym) 'any1 'any2 [. prg]) ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
any doFor(Context *CONTEXT_PTR, any x)
{
    mp_err _mp_error;
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

    y = car(x = cdr(x));
    if (!isCell(y) || !isCell(cdr(y)))
    {
        if (!isCell(y))
        {
            f->cnt = 1;
            f->bnd[0].sym = y;
            f->bnd[0].val = val(y);
        }
        else
        {
            f->cnt = 2;
            f->bnd[0].sym = cdr(y);
            f->bnd[0].val = val(cdr(y));
            f->bnd[1].sym = car(y);
            f->bnd[1].val = val(car(y));
            val(f->bnd[1].sym) = Nil;
            setCARType(f->bnd[1].sym, PTR_CELL);
        }

        y = Nil;
        x = cdr(x),  Push(c1, EVAL(CONTEXT_PTR, car(x)));

        if (isNum(data(c1)))
        {
            f->bnd[0].sym->cdr  = mkNum(CONTEXT_PTR, 0);
            setCARType(f->bnd[0].sym, PTR_CELL);
        }

        body = x = cdr(x);
        for (;;)
        {
            if (isNum(data(c1)))
            {
                if (! mp_cmp(num(f->bnd[0].sym->cdr), num(data(c1))))
                    break;

                mp_int *n = (mp_int*)malloc(sizeof(mp_int));
                _mp_error = mp_init(n); // TODO handle the errors appropriately

                _mp_error = mp_copy(num(f->bnd[0].sym->cdr), n);
                _mp_error = mp_incr(n);

                NewNumber( n, r);
                f->bnd[0].sym->cdr  = r;
                setCARType(f->bnd[0].sym, PTR_CELL);
            }
            else
            {
                if (isNil(data(c1)))
                {
                    break;
                }
                val(f->bnd[0].sym) = car(data(c1));
                setCARType(f->bnd[0].sym, PTR_CELL);
                if (isNil(data(c1) = cdr(data(c1))))
                {
                    data(c1) = Nil;
                }
            }
            do
            {
                if (!isNum(y = car(x)))
                {
                    if (isNil(car(y)))
                    {
                        y = cdr(y);
                        if (isNil(a = EVAL(CONTEXT_PTR, car(y))))
                        {
                            y = prog(CONTEXT_PTR, cdr(y));
                            goto for1;
                        }
                        val(At) = a;
                        setCARType(At, PTR_CELL);
                        y = Nil;
                    }
                    else if (car(y) == T)
                    {
                        y = cdr(y);
                        if (!isNil(a = EVAL(CONTEXT_PTR, car(y))))
                        {
                            val(At) = a;
                            setCARType(At, PTR_CELL);
                            y = prog(CONTEXT_PTR, cdr(y));
                            goto for1;
                        }
                        y = Nil;
                    }
                    else
                    {
                        y = evList(CONTEXT_PTR, y);
                    }
                }
            } while (!isNil(x = cdr(x)));
            x = body;
        }
for1:
        drop(c1);
        val(f->bnd[0].sym) = f->bnd[0].val;
        setCARType(f->bnd[0].sym, PTR_CELL);
        CONTEXT_PTR->Env.bind = f->link;
        free(f);
        return y;
    }

    return Nil;
}

// (while 'any . prg) -> any
any doWhile(Context *CONTEXT_PTR, any x)
{
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
        setCARType(y, PTR_CELL);
    }
    while (!isNil(x = cdr(x)));

    return val(y);
}


any doLoop(Context *CONTEXT_PTR, any ex)
{
   any x, y, a;

   for (;;) {
      x = cdr(ex);
      do {
         if (!isNil(y = car(x))) {
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
      } while (!isNil(x = cdr(x)));
   }
}


uword getHeapSize(Context *CONTEXT_PTR)
{
    int size = 0;
    int sizeFree = 0;
    heap *h = CONTEXT_PTR->Heaps;
    do
    {
        any p = h->cells + CELLS-1;
        do
        {
            size++;
        }
        while (--p >= h->cells);
    } while (h = h->next);

    any p = CONTEXT_PTR->Avail;
    while (p)
    {
        sizeFree++;
        p = car(p);
    }

    printf("MEM SIZE = %d FREE = %d\n", size, sizeFree);
    return size;
}


any doHS(Context *CONTEXT_PTR, any ignore)
{
    gc(CONTEXT_PTR, CELLS);
    getHeapSize(CONTEXT_PTR);
    return Nil;
}

any doSub(Context *CONTEXT_PTR, any ex)
{
    any x;
    cell c1, c2;
    mp_err _mp_error;

    x = cdr(ex);
    if (isNil(data(c1) = EVAL(CONTEXT_PTR, car(x))))
    {
        mp_int *id = (mp_int*)malloc(sizeof(mp_int));
        _mp_error = mp_init(id); // TODO handle the errors appropriately
        mp_set_i32(id, 0);

        NewNumber( id, idr);
        return idr;
    }

    NeedNum(ex, data(c1));

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(n); // TODO handle the errors appropriately
    _mp_error = mp_copy(num(data(c1)), n);

    NewNumber( n, r);
    Push(c1, r);

    while (!isNil(x = cdr(x)))
    {
        Push(c2, EVAL(CONTEXT_PTR, car(x)));
        if (isNil(data(c2)))
        {
            drop(c1);
            return Nil;
        }

        NeedNum(ex,data(c2));
        mp_int *m = num(data(c2));
        _mp_error = mp_sub(n, m, n);

        drop(c2);
    }

    return Pop(c1);
}

// (/ 'num ..) -> num
any doDiv(Context *CONTEXT_PTR, any ex)
{
    mp_err _mp_error;
    cell c1, c2;
    any x, y, z;

    x = cdr(ex);
    if (isNil(data(c1) = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex, data(c1));

    Push(c1, copyNum(CONTEXT_PTR, data(c1)));

    x = cdr(x);

    Push(c2, EVAL(CONTEXT_PTR, car(x)));
    if (isNil(data(c2)))
    {
        drop(c1);
        return Nil;
    }

    NeedNum(ex, data(c2));

    data(c2) = copyNum(CONTEXT_PTR, data(c2));
    mp_int *c = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(c);
    mp_int *d = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(d);
    _mp_error = mp_div(num(data(c1)), num(data(c2)), c, d);

    NewNumber( c, r1);
    data(c1) = r1;

    NewNumber( d, r2);
    data(c2) = r2;

    any result = cons(CONTEXT_PTR, data(c1), cons(CONTEXT_PTR, data(c2), Nil));

    Pop(c1);
    return result;
}

any doMul(Context *CONTEXT_PTR, any ex)
{
    any x;
    cell c1, c2;
    mp_err _mp_error;

    x = cdr(ex);
    if (isNil(data(c1) = EVAL(CONTEXT_PTR, car(x))))
    {
        mp_int *id = (mp_int*)malloc(sizeof(mp_int));
        _mp_error = mp_init(id); // TODO handle the errors appropriately
        mp_set_i32(id, 1);
        NewNumber( id, idr);
        return idr;
    }

    NeedNum(ex, data(c1));

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(n); // TODO handle the errors appropriately
    _mp_error = mp_copy(num(data(c1)), n);

    NewNumber( n, r);
    Push(c1, r);

    while (!isNil(x = cdr(x)))
    {
        Push(c2, EVAL(CONTEXT_PTR, car(x)));
        if (isNil(data(c2)))
        {
            drop(c1);
            return Nil;
        }

        NeedNum(ex,data(c2));
        mp_int *m = num(data(c2));
        _mp_error = mp_mul(n, m, n);

        drop(c2);
    }

    return Pop(c1);
}

// (** 'num ..) -> num
any doPow(Context *CONTEXT_PTR, any ex)
{
    mp_err _mp_error;
    any x, y;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(n);
    _mp_error = mp_copy(num(y), n);

    while (!isNil(x = cdr(x)))
    {
        if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
            return Nil;
        NeedNum(ex,y);

        int m = mp_get_i32(num(y));
        _mp_error = mp_expt_u32(n, m, n);

    }

    NewNumber( n, r);
    return r;
}

// (% 'num ..) -> num
any doMod(Context *CONTEXT_PTR, any ex)
{
    mp_err _mp_error;
    cell c1, c2;
    any x, y, z;

    x = cdr(ex);
    if (isNil(data(c1) = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex, data(c1));

    Push(c1, copyNum(CONTEXT_PTR, data(c1)));

    x = cdr(x);

    Push(c2, EVAL(CONTEXT_PTR, car(x)));
    if (isNil(data(c2)))
    {
        drop(c1);
        return Nil;
    }

    NeedNum(ex, data(c2));

    data(c2) = copyNum(CONTEXT_PTR, data(c2));
    mp_int *c = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(c);
    _mp_error = mp_div(num(data(c1)), num(data(c2)), NULL, c);

    NewNumber( c, r1);
    data(c1) = r1;

    return Pop(c1);
}

// (& 'num ..) -> num
any doBinNot(Context *CONTEXT_PTR, any ex)
{
    mp_err _mp_error;
    any x, y;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(n);
    _mp_error = mp_copy(num(y), n);

    _mp_error = mp_complement(n, n);

    NewNumber( n, r);
    return r;
}

// (& 'num ..) -> num
any doBinAnd(Context *CONTEXT_PTR, any ex)
{
    any x, y;
    mp_err _mp_error;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(n);
    _mp_error = mp_copy(num(y), n);

    while (!isNil(x = cdr(x)))
    {
        if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
            return Nil;
        NeedNum(ex,y);
        mp_int *m = num(y);
        _mp_error = mp_and(n, m, n);

    }

    NewNumber( n, r);
    return r;
}

// (| 'num ..) -> num
any doBinOr(Context *CONTEXT_PTR, any ex)
{
    any x, y;
    mp_err _mp_error;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(n);
    _mp_error = mp_copy(num(y), n);

    while (!isNil(x = cdr(x)))
    {
        if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
            return Nil;
        NeedNum(ex,y);
        mp_int *m = num(y);
        _mp_error = mp_or(n, m, n);

    }

    NewNumber( n, r);
    return r;
}

// (x| 'num ..) -> num
any doBinXor(Context *CONTEXT_PTR, any ex)
{
    mp_err _mp_error;
    any x, y;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(n);
    _mp_error = mp_copy(num(y), n);

    while (!isNil(x = cdr(x)))
    {
        if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
            return Nil;
        NeedNum(ex,y);
        mp_int *m = num(y);
        _mp_error = mp_xor(n, m, n);

    }

    NewNumber( n, r);
    return r;
}

// (>> 'num ..) -> num
any doBinRShift(Context *CONTEXT_PTR, any ex)
{
    mp_err _mp_error;
    any x, y;
    word s = 1;

    x = cdr(ex);
    any p1 = EVAL(CONTEXT_PTR, car(x));

    if (isNil(p1) || !isNum(p1)) return Nil;

    s = mp_get_i32(num(p1));

    x = cdr(x);
    any p2 = EVAL(CONTEXT_PTR, car(x));

    if (isNil(p2) || !isNum(p2)) return Nil;

    mp_int *m = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(m);
    if (s >= 0)
    {
        _mp_error = mp_div_2d(num(p2), s, m, NULL);
    }
    else
    {
        s *= -1;
        _mp_error = mp_mul_2d(num(p2), s, m);
    }

    NewNumber( m, r);

    return r;
}

// (+ 'num ..) -> num
any doNumLt(Context *CONTEXT_PTR, any ex)
{
    mp_err _mp_error;
    any x, y;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(n);
    _mp_error = mp_copy(num(y), n);

    while (!isNil(x = cdr(x)))
    {
        if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
            return Nil;
        NeedNum(ex,y);
        mp_int *m = num(y);
        if (MP_LT != mp_cmp(n, m))
        {
            mp_clear(n);
            free(n);
            return Nil;
        }

    }

    mp_clear(n);
    free(n);
    return T;
}


// (+ 'num ..) -> num
any doNumGt(Context *CONTEXT_PTR, any ex)
{
    mp_err _mp_error;
    any x, y;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);

    MP_INT *n = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(n);
    mpz_set(n, num(y));

    while (!isNil(x = cdr(x)))
    {
        if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
            return Nil;
        NeedNum(ex,y);
        MP_INT *m = num(y);
        if (mpz_cmp(n, m) <= 0)
        {
            mpz_clear(n);
            free(n);
            return Nil;
        }

    }

    mpz_clear(n);
    free(n);
    return T;
}



// (+ 'num ..) -> num
any doAdd(Context *CONTEXT_PTR, any ex)
{
    any x;
    cell c1, c2;

    x = cdr(ex);
    if (isNil(data(c1) = EVAL(CONTEXT_PTR, car(x))))
    {
        MP_INT *id = (MP_INT*)malloc(sizeof(MP_INT));
        mpz_init(id);
        mpz_set_ui(id, 0);
        NewNumber(id, idr);
        return idr;
    }

    NeedNum(ex, data(c1));

    MP_INT *n = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(n);
    mpz_set(n, num(data(c1)));

    NewNumber( n, r);
    Push(c1, r);

    while (!isNil(x = cdr(x)))
    {
        Push(c2, EVAL(CONTEXT_PTR, car(x)));
        if (isNil(data(c2)))
        {
            drop(c1);
            return Nil;
        }

        NeedNum(ex,data(c2));
        MP_INT *m = num(data(c2));
        mpz_add(n, m, n);

        drop(c2);
    }

    return Pop(c1);
}

// (inc 'num ..) -> num
any doInc(Context *CONTEXT_PTR, any ex)
{
    any x;
    cell c1, c2;

    x = cdr(ex);
    if (isNil(data(c1) = EVAL(CONTEXT_PTR, car(x))))
        return Nil;

    NeedNum(ex, data(c1));

    MP_INT *n = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(n); // TODO handle the errors appropriately
    mpz_add_ui(n, num(data(c1)), 1);


    NewNumber( n, r);

    return r;
}

// (inc 'num ..) -> num
any doDec(Context *CONTEXT_PTR, any ex)
{
    any x;
    cell c1, c2;

    x = cdr(ex);
    if (isNil(data(c1) = EVAL(CONTEXT_PTR, car(x))))
        return Nil;

    NeedNum(ex, data(c1));

    MP_INT *n = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(n); 
    mpz_sub_ui(n, num(data(c1)), 1);

    NewNumber( n, r);

    return r;
}

// (rand 'num ..) -> num
any doRandom(Context *CONTEXT_PTR, any ex)
{
    any x, y;
    uword s = 32;
    MP_INT *n = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(n);

    x = cdr(ex);
    if (!isNil(y = EVAL(CONTEXT_PTR, car(x))))
    {
        mpz_set(n, num(y));
        s = mpz_get_ui(n);
    }

    mpz_random(n, s);

    NewNumber( n, r);
    return r;
}

any copyNum(Context *CONTEXT_PTR, any n)
{
    if (!isNum(n)) return Nil;

    MP_INT *BIGNUM = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(BIGNUM);
    mpz_set(BIGNUM, num(n));

    NewNumber(BIGNUM, r);
    return r;
}

void releaseExtNum(external *p)
{
    if (p->type != EXT_NUM)
    {
        fprintf(stderr, "Not a number %d\n", p->type);
        exit(0);
    }

    mpz_clear((mp_int*)p->pointer);
    free(p->pointer);
    free(p);
}

external * copyExtNum(Context *CONTEXT_PTR, external *ext)
{
    if (ext->type != EXT_NUM)
    {
        fprintf(stderr, "Not a number\n");
        exit(0);
    }

    MP_INT *BIGNUM = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(BIGNUM);
    mpz_set(BIGNUM, (MP_INT*)ext->pointer);

    external *e = (external *)malloc(sizeof(external));
    e->type = EXT_NUM;
    e->release = releaseExtNum;
    e->print = printExtNum;
    e->copy = copyExtNum;
    e->equal = equalExtNum;
    e->pointer = (void*)(BIGNUM);

    return e;
}

int equalExtNum(Context *CONTEXT_PTR, external*x, external*y)
{
    if (x->type != EXT_NUM)
    {
        fprintf(stderr, "LHS is not number\n");
        return 1;
    }

    if (y->type != EXT_NUM)
    {
        fprintf(stderr, "RHS is not number\n");
        return 1;
    }

    return mpz_cmp((MP_INT*)x->pointer, (MP_INT*)y->pointer);
}

char * printExtNum(Context *CONTEXT_PTR, struct _external* obj)
{
    return mpz_get_str(NULL, 10, (MP_INT*)obj->pointer);
}

// (++ var) -> any
any doPopq(Context *CONTEXT_PTR, any ex)
{
    any p1 = cadr(ex);

    if (!isSym(p1))
    {
        return p1;
    }

    any theList = cdr(p1);

    any r = cdr(theList);

    cdr(p1) = r;

    return car(theList);
}

any indx(Context *CONTEXT_PTR, any x, any y)
{
    any z = y;

    MP_INT *n = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(n);
    mpz_set_ui(n, 1);

    while (!isNil(y))
    {
        if (0 == equal(CONTEXT_PTR, x, car(y)))
        {
            NewNumber( n, r);
            return r;
        }
        mpz_add_ui(n, n, 1);
        if (z == (y = cdr(y)))
            return Nil;
    }
    return Nil;
}

// (index 'any 'lst) -> cnt | NIL
any doIndex(Context *CONTEXT_PTR, any x)
{
   any n;
   cell c1;

   x = cdr(x);
   Push(c1, EVAL(CONTEXT_PTR, car(x)));

   x = cdr(x);
   x = EVAL(CONTEXT_PTR, car(x));

   if (isNil(x))
   {
       Pop(c1);
       return Nil;
   }
   if (!isCell(x))
   {
       Pop(c1);
       return Nil;
   }

   return indx(CONTEXT_PTR, Pop(c1), x);
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
        any c = cons(CONTEXT_PTR, y, Nil);

        c->type = PTR_CELL;

        *CONTEXT_PTR->Env.make = c;
        
        CONTEXT_PTR->Env.make = &cdr(c);

    }
    while (!isNil(x = cdr(x)));
    return y;
}

// (length 'any) -> cnt | T
any doLength(Context *CONTEXT_PTR, any x)
{
    uword w;
    int n, c;
    any y;
    int lengthBiggerThanZero=0;

    x = EVAL(CONTEXT_PTR, cadr(x));
    CellPartType t = GetType(x);
    MP_INT *r = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(r); // TODO handle the errors appropriately
    mpz_set_ui(r, 0);

    if (isNil(x))
    {
        mpz_clear(r);
        free(r);
        return Nil;
    }
    if (isNum(x))
    {
        mpz_clear(r);
        free(r);
        return x;
    }
    else if (isSym(x))
    {
        x = car(x);
        while (!isNil(x))
        {
            w = (uword)(car(x));
            if (w) lengthBiggerThanZero = 1;
            while (w)
            {
                mpz_add_ui(r, r, 1);
                w >>= 8;
            }
            x = x->cdr;
        }
    }
    else if (isCell(x))
    {
        lengthBiggerThanZero = 1;
        while (!isNil(x))
        {
            mpz_add_ui(r, r, 1);
            x = cdr(x);
        }
    }
    else
    {
        mpz_clear(r);
        free(r);
        return Nil;
    }

    if (!lengthBiggerThanZero)
    {
        mpz_clear(r);
        free(r);
        return Nil;
    }

    NewNumber( r, l);
    return l;
}

// (list 'any ['any ..]) -> lst
any doList(Context *CONTEXT_PTR, any x)
{
   any y;
   cell c1;

   x = cdr(x);
   Push(c1, y = cons(CONTEXT_PTR, EVAL(CONTEXT_PTR, car(x)),Nil));
   while (!isNil(x = cdr(x)))
   {
      cdr(y) = cons(CONTEXT_PTR, EVAL(CONTEXT_PTR, car(x)),Nil);
      setCARType(y, PTR_CELL);
      y = cdr(y);
   }
   return Pop(c1);
}

// (mapcar 'fun 'lst ..) -> lst
any doMapcar(Context *CONTEXT_PTR, any ex)
{
   any x = cdr(ex);
   cell res, foo;

   Push(res, Nil);
   Push(foo, EVAL(CONTEXT_PTR, car(x)));

   x = cdr(x);
   if (isCell(x)) {
      int i, n = 0;
      //cell c[length(CONTEXT_PTR, x)];
      cell *c = (cell *)calloc(sizeof(cell), length(CONTEXT_PTR, x));

      do
         Push(c[n], EVAL(CONTEXT_PTR, car(x))), ++n;
      while (!isNil(x = cdr(x)));
      if (!isCell(data(c[0])))
      {
          free(c);
         return Pop(res);
      }
      data(res) = x = cons(CONTEXT_PTR, apply(CONTEXT_PTR, ex, data(foo), YES, n, c), Nil);
      while (!isNil(data(c[0]) = cdr(data(c[0])))) {
         for (i = 1; i < n; ++i)
            data(c[i]) = cdr(data(c[i]));
         cdr(x) = cons(CONTEXT_PTR, apply(CONTEXT_PTR, ex, data(foo), YES, n, c), Nil);
         setCARType(x, PTR_CELL);
         x = cdr(x);
      }

      free(c);
   }

   return Pop(res);
}

// (make .. [(made 'lst ..)] .. [(link 'any ..)] ..) -> any
any doMake(Context *CONTEXT_PTR, any x)
{
    any *make, *yoke;
    cell c1;

    Push(c1, Nil);
    make = CONTEXT_PTR->Env.make;
    CONTEXT_PTR->Env.make = &data(c1);

    while (!isNil(x = cdr(x)))
    {
        if (isCell(car(x)))
        {
            evList(CONTEXT_PTR, car(x));
        }
    }

    CONTEXT_PTR->Env.make = make;
    return Pop(c1);
}

any doCons(Context *CONTEXT_PTR, any x)
{
   any y;
   cell c1;

   x = cdr(x);
   Push(c1, y = cons(CONTEXT_PTR, EVAL(CONTEXT_PTR, car(x)),Nil));
   while (!isNil(cdr(x = cdr(x))))
   {
      y = cdr(y) = cons(CONTEXT_PTR, EVAL(CONTEXT_PTR, car(x)),Nil);
   }
   cdr(y) = EVAL(CONTEXT_PTR, car(x));
   setCARType(y, PTR_CELL);
   return Pop(c1);
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

any apply(Context *CONTEXT_PTR, any ex, any foo, bool cf, int n, cell *p)
{
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

         f->link = CONTEXT_PTR->Env.bind,  CONTEXT_PTR->Env.bind = f;
         f->i = 0;
         f->cnt = 1,  f->bnd[0].sym = At,  f->bnd[0].val = val(At);
         while (!isNil(x)) {
            f->bnd[f->cnt].val = val(f->bnd[f->cnt].sym = car(x));
            val(f->bnd[f->cnt].sym) = --n<0? Nil : cf? car(data(p[f->cnt-1])) : data(p[f->cnt-1]);
            setCARType(f->bnd[f->cnt].sym, PTR_CELL);
            ++f->cnt, x = cdr(x);
         }
         if (isNil(x))
            x = prog(CONTEXT_PTR, cdr(foo));
         else if (x != At) {
            f->bnd[f->cnt].sym = x;
            f->bnd[f->cnt].val = val(x);
            val(x) = Nil;
            setCARType(x, PTR_CELL);
            while (--n >= 0)
            {
               val(x) = cons(CONTEXT_PTR, consSym(CONTEXT_PTR, cf? car(data(p[n+f->cnt-1])) : data(p[n+f->cnt-1]), 0), val(x));
               setCARType(x, PTR_CELL);
            }
            ++f->cnt;
            x = prog(CONTEXT_PTR, cdr(foo));
         }
         else {
            int cnt = n;
            int next = CONTEXT_PTR->Env.next;
            cell *arg = CONTEXT_PTR->Env.arg;
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

      if (isNil(val(foo)) || foo == val(foo))
         return undefined(CONTEXT_PTR, foo, ex);
      foo = val(foo);
   }
   if (--n < 0)
   {
      cdr(CONTEXT_PTR->ApplyBody) = Nil;
      setCARType(CONTEXT_PTR->ApplyBody, PTR_CELL);
   }
   else {
      any x = CONTEXT_PTR->ApplyArgs;
      val(caar(x)) = cf? car(data(p[n])) : data(p[n]);
      while (--n >= 0) {
         if (!isCell(cdr(x)))
         {
            cdr(x) = cons(CONTEXT_PTR, cons(CONTEXT_PTR, consSym(CONTEXT_PTR, Nil,0), car(x)), Nil);
            setCARType(x, PTR_CELL);
         }
         x = cdr(x);
         val(caar(x)) = cf? car(data(p[n])) : data(p[n]);
         setCARType(caar(x), PTR_CELL);
      }
      cdr(CONTEXT_PTR->ApplyBody) = car(x);
      setCARType(CONTEXT_PTR->ApplyBody, PTR_CELL);
   }

   return evSubr(car(foo), CONTEXT_PTR->ApplyBody);
}

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

void getParse(Context *CONTEXT_PTR)
{
   if ((CONTEXT_PTR->Chr = getByte(CONTEXT_PTR, &CONTEXT_PTR->Env.parser->i, &CONTEXT_PTR->Env.parser->w, &CONTEXT_PTR->Env.parser->nm)) == 0)
      CONTEXT_PTR->Chr = ']';
}


any parse(Context *CONTEXT_PTR, any x, bool skp)
{
   int c;
   parseFrame *save, parser;
   void (*getSave)(Context*);
   cell c1;

   save = CONTEXT_PTR->Env.parser;
   CONTEXT_PTR->Env.parser = &parser;
   parser.nm = parser.sym = x;
   getSave = CONTEXT_PTR->Env.get, CONTEXT_PTR->Env.get = getParse,  c = CONTEXT_PTR->Chr;
   Push(c1, CONTEXT_PTR->Env.parser->sym);
   CONTEXT_PTR->Chr = getByte1(CONTEXT_PTR, &parser.i, &parser.w, &parser.nm);
   if (skp)
   {
      getParse(CONTEXT_PTR);
   }
   x = rdList(CONTEXT_PTR);
   drop(c1);
   CONTEXT_PTR->Chr = c,  CONTEXT_PTR->Env.get = getSave,  CONTEXT_PTR->Env.parser = save;
   return x;
}

any load(Context *CONTEXT_PTR, any ex, int pr, any x)
{
    cell c1, c2;
    inFrame f;

    // Handle command line arguments to execute function
    
    if (isSym(x))
    {
        word c = firstByte(CONTEXT_PTR, x);

        if (c == '-' || c == '(')
        {
            Push(c1, parse(CONTEXT_PTR, x, YES));
            x = evList(CONTEXT_PTR, data(c1));
            drop(c1);
            return x;
        }
    }

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
        {
            // TODO - WHY @ does not work in files
            x = EVAL(CONTEXT_PTR, data(c1));
        }
        else
        {
            Push(c2, val(At));
            x = EVAL(CONTEXT_PTR, data(c1));
            cdr(At) = x;
            setCARType(At, PTR_CELL);
            //x = val(At) = EVAL(CONTEXT_PTR, data(c1));

            cdr(At2) = c2.car;
            setCARType(At2, PTR_CELL);

            cdr(At3) = cdr(At2);
            setCARType(At3, PTR_CELL);

            //val(At3) = val(At2),  val(At2) = data(c2);
            outString(CONTEXT_PTR, "-> ");
            fflush(CONTEXT_PTR->OutFile);
            print(CONTEXT_PTR, x);
            newline(CONTEXT_PTR);

        }
        drop(c1);
        dump("load");
    }

    return ex;
}

any loadAll(Context *CONTEXT_PTR, any ex)
{
   any x = Nil;

   while (*CONTEXT_PTR->AV  &&  strcmp(*CONTEXT_PTR->AV,"-") != 0)
      x = load(CONTEXT_PTR, ex, 0, mkStr(CONTEXT_PTR, *CONTEXT_PTR->AV++));
   return x;
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
   } while (!isNil(x = cdr(x)));
   return y;
}

void pushOutFiles(Context *CONTEXT_PTR, outFrame *f)
{
    CONTEXT_PTR->OutFile = f->fp;
    f->put = CONTEXT_PTR->Env.put,  CONTEXT_PTR->Env.put = putStdout;
    f->link = CONTEXT_PTR->Env.outFrames,  CONTEXT_PTR->Env.outFrames = f;
}

void rdOpen(Context *CONTEXT_PTR, any ex, any x, inFrame *f)
{
    if (isNil(x))
    {
        f->fp = stdin;
    }
    else
    {
        int ps = pathSize(CONTEXT_PTR, x);
        char *nm = (char*)malloc(ps);

        pathString(CONTEXT_PTR, x,nm);

        if (!(f->fp = fopen(nm, "rb")))
        {
            openErr(ex, nm);
        }

        free(nm);
    }
}

// (prin 'any ..) -> any
any doPrin(Context *CONTEXT_PTR, any x)
{
   any y = Nil;

   while (!isNil(x = cdr(x)) )
   {
      prin(CONTEXT_PTR, y = EVAL(CONTEXT_PTR, car(x)));
   }
   newline(CONTEXT_PTR);
   return y;
}

void getStdin(Context *CONTEXT_PTR)
{
    CONTEXT_PTR->Chr = getc(CONTEXT_PTR->InFile);
}


any doIn(Context *CONTEXT_PTR, any ex)
{
   any x;
   inFrame f;

   x = cdr(ex),  x = EVAL(CONTEXT_PTR, car(x));
   rdOpen(CONTEXT_PTR, ex,x,&f);
   pushInFiles(CONTEXT_PTR, &f);
   x = prog(CONTEXT_PTR, cddr(ex));
   popInFiles(CONTEXT_PTR);
   return x;
}

void space(Context *CONTEXT_PTR)
{
    CONTEXT_PTR->Env.put(CONTEXT_PTR, ' ');
}

// (line 'flg) -> lst|sym
any doLine(Context *CONTEXT_PTR, any x)
{
   any y;
   int i;
   uword w;
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
         any c = mkChar(CONTEXT_PTR, CONTEXT_PTR->Chr);
         cdr(y) = cons(CONTEXT_PTR, c, Nil);
         setCARType(y, PTR_CELL);
         y = cdr(y);
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

void pushInFiles(Context *CONTEXT_PTR, inFrame *f)
{
    f->next = CONTEXT_PTR->Chr,  CONTEXT_PTR->Chr = 0;
    CONTEXT_PTR->InFile = f->fp;
    f->get = CONTEXT_PTR->Env.get,  CONTEXT_PTR->Env.get = getStdin;
    f->link = CONTEXT_PTR->Env.inFrames,  CONTEXT_PTR->Env.inFrames = f;
}

// (out 'any . prg) -> any
any doOut(Context *CONTEXT_PTR, any ex)
{
   any x;
   outFrame f;

   x = cdr(ex),  x = EVAL(CONTEXT_PTR, car(x));
   wrOpen(CONTEXT_PTR, ex,x,&f);
   pushOutFiles(CONTEXT_PTR, &f);
   x = prog(CONTEXT_PTR, cddr(ex));
   popOutFiles(CONTEXT_PTR);
   return x;
}

void newline(Context *CONTEXT_PTR)
{
    CONTEXT_PTR->Env.put(CONTEXT_PTR, '\n');
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

// (char) -> sym
// (char 'num) -> sym
// (char 'sym) -> num
any doChar(Context *CONTEXT_PTR, any ex)
{
    any x = cdr(ex);

    if (isNil(x))
    {
        if (!CONTEXT_PTR->Chr)
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
        }

        x = CONTEXT_PTR->Chr < 0 ? Nil : mkChar(CONTEXT_PTR, CONTEXT_PTR->Chr);
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        return x;
    }

    x = EVAL(CONTEXT_PTR, car(x));

    if (isNum(x))
    {
        return mkChar(CONTEXT_PTR, mp_get_i32(num(x)));
    }

    if (isSym(x))
    {

        MP_INT *n = (MP_INT*)malloc(sizeof(MP_INT));
        mpz_init(n);
        mpz_set_ui(n, firstByte(CONTEXT_PTR, x));

        NewNumber(n, r);
        return r;
    }
    return Nil;
}

any doSwitchBase(Context *CONTEXT_PTR, any ex)
{
    any p1 = cadr(ex);
    any p2 = caddr(ex);
    int base = 10;

    p1 = EVAL(CONTEXT_PTR, p1);
    CellPartType t = GetType(p1);

    p2 = EVAL(CONTEXT_PTR, p2);
    if (isNum(p2))
    {
        base = mpz_get_ui(num(p2));
    }

    if (isNum(p1))
    {
        char *buf = mpz_get_str(NULL, base, num(p1));
        any r = mkSym(CONTEXT_PTR, (byte*)buf);
        free(buf);
        return r;
    }
    else if (isSym(p1))
    {
        int LEN = pathSize(CONTEXT_PTR, p1);
        int CTR = 0;
        char *str = (char *)calloc(LEN, 1);
        sym2str(CONTEXT_PTR, p1, str);

        MP_INT *BIGNUM = (MP_INT*)malloc(sizeof(MP_INT));
        mpz_init(BIGNUM);
        mpz_init_set_str(BIGNUM, str, base);
        free(str);

        NewNumber( BIGNUM, r);
        return r;
    }

    return Nil;
}


any doRd(Context *CONTEXT_PTR, any ex)
{
    int EndReached = 0;
    any params = cdr(ex);
    any p1 = car(params);
    any p2 = cadr(params);

    p1 = EVAL(CONTEXT_PTR, p1);
    if (!isNum(p1)) return Nil;
    size_t count = mpz_get_ui(num(p1));


    mp_order order = MP_LSB_FIRST;
    p2 = EVAL(CONTEXT_PTR, p2);
    if (isNum(p2))
    {
        if (mpz_get_ui(num(p2)) == 1) order = MP_MSB_FIRST;
    }


    MP_INT *n = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(n);

    MP_INT *temp = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(temp);
    // TODO use bignum instead of int for count?
    for(int i = 0;i < count; i++)
    {
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        if (CONTEXT_PTR->Chr < 0 )
        {
            EndReached = 1;
            count = i;
            break;
        }

        if (i == 0)
        {
            mpz_set_ui(n, CONTEXT_PTR->Chr);
        }
        else
        {
            if (order == 1)
            {
                mpz_mul_ui(n, n, 256);
                mpz_add_ui(n, n, CONTEXT_PTR->Chr);
            }
            else
            {
                mpz_set_ui(temp, CONTEXT_PTR->Chr);
                for (int j = 0; j < i; j++)
                {
                    mpz_mul_ui(temp, temp, 256);
                }
                mpz_add(n, n, temp);
            }
        }
    }
    mpz_clear(temp);
    free(temp);


    NewNumber(n, r);
    cell c1, c2;
    Push(c1, r);
    Push(c2, cons(CONTEXT_PTR, EndReached? Nil: T, Nil));
    any rr = cons(CONTEXT_PTR, r, data(c2));
    drop(c1);

    return rr;
}

any doWr(Context *CONTEXT_PTR, any ex)
{
    mp_err _mp_error;
    any params = cdr(ex);
    any p1 = car(params);
    any p2 = cadr(params);
    any p3 = caddr(params);

    p1 = EVAL(CONTEXT_PTR, p1);
    if (!isNum(p1)) return Nil;
    size_t count = mp_pack_count(num(p1), 0, 1 );

    if (count == 0)
    {
        CONTEXT_PTR->Env.put(CONTEXT_PTR, 0);
        mp_int *n = (mp_int*)malloc(sizeof(mp_int));
        _mp_error = mp_init(n); // TODO handle the errors appropriately
        mp_set(n, 1);

        NewNumber( n, r);
        return r;
    }


    mp_order order = MP_LSB_FIRST;
    p2 = EVAL(CONTEXT_PTR, p2);
    if (isNum(p2))
    {
        if (mp_get_i32(num(p2)) == 1) order = MP_MSB_FIRST;
    }

    mp_endian endianess = MP_BIG_ENDIAN;
    p3 = EVAL(CONTEXT_PTR, p3);
    if (isNum(p3))
    {
        if (mp_get_i32(num(p3)) == 1) endianess = MP_LITTLE_ENDIAN;
    }

    size_t written;
    unsigned char *buf = (char *)malloc(count);
    _mp_error = mp_pack((void *)buf, count, &written, order, 1, endianess, 0, num(p1));

    for (int i = 0; i < count; i++)
    {
        CONTEXT_PTR->Env.put(CONTEXT_PTR, buf[i]);
    }
    free(buf);

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    _mp_error = mp_init(n); // TODO handle the errors appropriately
    mp_set(n, written);

    NewNumber( n, r);
    return r;
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

void putStdout(Context *CONTEXT_PTR, int c)
{
    putc(c, CONTEXT_PTR->OutFile);
}

void outString(Context *CONTEXT_PTR, char *s)
{
    while (*s)
        CONTEXT_PTR->Env.put(CONTEXT_PTR, *s++);
}

// (bye 'num|NIL)
any doBye(Context *CONTEXT_PTR, any ex)
{
   bye(0);
   return ex;
}


any doCall(Context *CONTEXT_PTR, any ex)
{
    any y;
    any x = cdr(ex);

    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
    {
        return Nil;
    }

    if (!isSym(y))
    {
        return Nil;
    }

    int len = pathSize(CONTEXT_PTR, y);
    char *buf = (char *)calloc(len, 1);
    sym2str(CONTEXT_PTR, y, buf);
    int ret = system(buf);
    free(buf);

    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    mp_err _mp_error = mp_init(n); // TODO handle the errors appropriately
    mp_set_i32(n, ret);

    NewNumber( n, r);

    return r;
}

void prin(Context *CONTEXT_PTR, any x)
{
    if (isNil(x))
    {
        printf("Nil");
        return;
    }

    if (!isNil(x))
    {
        if (isNum(x))
        {
            outNum(CONTEXT_PTR, x);
        }
        else if (x == T)
        {
            printf("T");
        }
        else if (isSym(x))
        {
            printLongTXT(CONTEXT_PTR, x);

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

void outNum(Context *CONTEXT_PTR, any n)
{
    int len;
    mp_err _mp_error = mp_radix_size((mp_int*)car(n), 10, &len);
    char *buf = (char*)malloc(len);
    _mp_error = mp_to_radix(num(n), buf, len, NULL, 10);
    outString(CONTEXT_PTR, buf);
    free(buf);
}

void print(Context *CONTEXT_PTR, any x)
{
    if (x == T)
    {
        CONTEXT_PTR->Env.put(CONTEXT_PTR, 'T');
        return;
    }

    if (isNil(x))
    {
        outString(CONTEXT_PTR, "Nil");
        return;
    }

    if (isNum(x))
    {
        //outNum(CONTEXT_PTR, x);
        //return;
    }
    if (isSym(x))
    {
        int quotedText = NULL != isIntern(CONTEXT_PTR, x, CONTEXT_PTR->Transient);
        if (quotedText) CONTEXT_PTR->Env.put(CONTEXT_PTR, '"');
        printLongTXT(CONTEXT_PTR, x);
        if (quotedText) CONTEXT_PTR->Env.put(CONTEXT_PTR, '"');
        return;
    }

    if (GetType(x) == EXT)
    {
        external *e = (external*)car(x);
        char *b = e->print(CONTEXT_PTR, e);
        outString(CONTEXT_PTR, b);
        free(b);
        return;
    }

    if (isCell(x))
    {
        CONTEXT_PTR->Env.put(CONTEXT_PTR, '(');
        print(CONTEXT_PTR, car(x));
        while (!isNil(x = cdr(x)))
        {
            CONTEXT_PTR->Env.put(CONTEXT_PTR, ' ');
            if (isCell(x))
            {
                print(CONTEXT_PTR, car(x));
            }
            else
            {
                print(CONTEXT_PTR, x);
            }
        }
        CONTEXT_PTR->Env.put(CONTEXT_PTR, ')');
        return;
    }

    if (GetType(x) == FUNC)
    {
        char buf[256];
        sprintf (buf, "C FUNCTION %p", (void*)x);
        outString(CONTEXT_PTR, buf);
        return;
    }

    fprintf (stderr, "TODO NOT A NUMBER %p %p\n", (void*)x, (void*)Nil);
    return;
}

void printLongTXT(Context *CONTEXT_PTR, any nm)
{
    int i, c;
    uword w;

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

int pathSize(Context *CONTEXT_PTR, any x)
{
    return symBytes(CONTEXT_PTR, x) + 1;
}

static char Delim[] = " \t\n\r\"'(),[]`~{}";

any read0(Context *CONTEXT_PTR, bool top)
{
    int i;
    uword w;
    any x, y;
    cell c1, *p;

    if (skip(CONTEXT_PTR) < 0)
    {
        if (top)
            return Nil;
        eofErr();
    }
    if (CONTEXT_PTR->Chr == '(')
    {
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        x = rdList(CONTEXT_PTR);
        if (top  &&  CONTEXT_PTR->Chr == ']')
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
        return x;
    }
    if (CONTEXT_PTR->Chr == '[')
    {
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        x = rdList(CONTEXT_PTR);
        if (CONTEXT_PTR->Chr != ']')
            err(NULL, x, "Super parentheses mismatch");
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        return x;
    }
    if (CONTEXT_PTR->Chr == '\'')
    {
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        any rest = read0(CONTEXT_PTR, top);
        return cons(CONTEXT_PTR, doQuote_D, rest);
    }
    if (CONTEXT_PTR->Chr == ',')
    {
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        return read0(CONTEXT_PTR, top);
    }
    if (CONTEXT_PTR->Chr == '`')
    {
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        Push(c1, read0(CONTEXT_PTR, top));
        x = EVAL(CONTEXT_PTR, data(c1));
        drop(c1);
        return x;
    }
    if (CONTEXT_PTR->Chr == '"')
    {
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        if (CONTEXT_PTR->Chr == '"')
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
            return Nil;
        }
        if (!testEsc(CONTEXT_PTR))
            eofErr();
        putByte1(CONTEXT_PTR->Chr, &i, &w, &p);
        while (CONTEXT_PTR->Env.get(CONTEXT_PTR), CONTEXT_PTR->Chr != '"')
        {
            if (!testEsc(CONTEXT_PTR))
                eofErr();
            putByte(CONTEXT_PTR, CONTEXT_PTR->Chr, &i, &w, &p, &c1);
        }
        y = popSym(CONTEXT_PTR, i, w, p, &c1),  CONTEXT_PTR->Env.get(CONTEXT_PTR);
        if (x = isIntern(CONTEXT_PTR, tail(y), CONTEXT_PTR->Transient))
            return x;
        intern(CONTEXT_PTR, y, CONTEXT_PTR->Transient);
        return y;
    }
    if (strchr(Delim, CONTEXT_PTR->Chr))
        err(NULL, NULL, "Bad input '%c' (%d)", isprint(CONTEXT_PTR->Chr)? CONTEXT_PTR->Chr:'?', CONTEXT_PTR->Chr);
    if (CONTEXT_PTR->Chr == '\\')
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
    putByte1(CONTEXT_PTR->Chr, &i, &w, &p);

    int count=0;
    for (;;)
    {
        count++;
        // if (count > 6)
        // {
        //     printf("%s too long\n", (char*)&w);
        //     bye(0);
        // }
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        if (strchr(Delim, CONTEXT_PTR->Chr))
        {
            break;
        }
        if (CONTEXT_PTR->Chr == '\\')
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
        }
        putByte(CONTEXT_PTR, CONTEXT_PTR->Chr, &i, &w, &p, &c1);
    }

    dump("putbyte1");
    y = popSym(CONTEXT_PTR, i, w, p, &c1);
    dump("putbyte2");
    if (x = symToNum(CONTEXT_PTR, tail(y), 0, '.', 0))
    {
        return x;
    }
    if (x = isIntern(CONTEXT_PTR, tail(y), CONTEXT_PTR->Intern))
    {
        return x;
    }

    intern(CONTEXT_PTR, y, CONTEXT_PTR->Intern);
    val(y) = Nil;
    setCARType(y, PTR_CELL);
    return y;
}

any read1(Context *CONTEXT_PTR, int end)
{
   if (!CONTEXT_PTR->Chr)
      CONTEXT_PTR->Env.get(CONTEXT_PTR);
   if (CONTEXT_PTR->Chr == end)
      return Nil;
   return read0(CONTEXT_PTR, YES);
}

bool testEsc(Context *CONTEXT_PTR)
{
    for (;;)
    {
        if (CONTEXT_PTR->Chr < 0)
            return NO;
        if (CONTEXT_PTR->Chr != '\\')
            return YES;
        if (CONTEXT_PTR->Env.get(CONTEXT_PTR), CONTEXT_PTR->Chr != '\n')
            return YES;
        do
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
        }
        while (CONTEXT_PTR->Chr == ' '  ||  CONTEXT_PTR->Chr == '\t');
    }
}

any rdList(Context *CONTEXT_PTR)
{
    any x;
    cell c1;

    for (;;)
    {
        if (skip(CONTEXT_PTR) == ')')
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
            return Nil;
        }
        if (CONTEXT_PTR->Chr == ']')
        {
            return Nil;
        }
        if (CONTEXT_PTR->Chr != '~')
        {
            x = cons(CONTEXT_PTR, read0(CONTEXT_PTR, NO),Nil);
            Push(c1, x);
            break;
        }
        CONTEXT_PTR->Env.get(CONTEXT_PTR);

        x = read0(CONTEXT_PTR, NO);
        Push(c1, x);

        x = data(c1) = EVAL(CONTEXT_PTR, data(c1));
        if (isCell(x) && !isNil(x))
        {
            while (!isNil(cdr(x)) && isCell(cdr(x)))
            {
                x = cdr(x);
            }
            break;
        }
        drop(c1);
    }

    for (;;)
    {
        if (skip(CONTEXT_PTR) == ')')
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
            break;
        }
        if (CONTEXT_PTR->Chr == ']')
            break;
        if (CONTEXT_PTR->Chr == '.')
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
            cdr(x) = skip(CONTEXT_PTR)==')' || CONTEXT_PTR->Chr==']'? data(c1) : read0(CONTEXT_PTR, NO);
            if (skip(CONTEXT_PTR) == ')')
                CONTEXT_PTR->Env.get(CONTEXT_PTR);
            else if (CONTEXT_PTR->Chr != ']')
                err(NULL, x, "Bad dotted pair");
            break;
        }
        if (CONTEXT_PTR->Chr != '~')
        {
            cdr(x) = cons(CONTEXT_PTR, read0(CONTEXT_PTR, NO),Nil);
            setCARType(x, PTR_CELL);
            x = cdr(x);
        }
        else
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
            cdr(x) = read0(CONTEXT_PTR, NO);
            setCARType(x, PTR_CELL);
            cdr(x) = EVAL(CONTEXT_PTR, cdr(x));
            setCARType(x, PTR_CELL);
            while (!isNil(cdr(x)) && isCell(cdr(x)))
            {
                x = cdr(x);
            }
        }
    }
    return Pop(c1);
}

bool eol(Context *CONTEXT_PTR)
{
   if (CONTEXT_PTR->Chr < 0)
   {
      return YES;
   }

   if (CONTEXT_PTR->Chr == '\n')
   {
      CONTEXT_PTR->Chr = 0;
      return YES;
   }

   if (CONTEXT_PTR->Chr == '\r')
   {
      CONTEXT_PTR->Env.get(CONTEXT_PTR);
      if (CONTEXT_PTR->Chr == '\n')
      {
         CONTEXT_PTR->Chr = 0;
      }
      return YES;
   }

   return NO;
}

void comment(Context *CONTEXT_PTR)
{
    CONTEXT_PTR->Env.get(CONTEXT_PTR);
    if (CONTEXT_PTR->Chr != '{')
    {
        while (CONTEXT_PTR->Chr != '\n')
        {
            if (CONTEXT_PTR->Chr < 0)
            {
                return;
            }
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
        }
    }
    else
    {
        int n = 0;

        for (;;) {  // #{block-comment}# from Kriangkrai Soatthiyanont
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
            if (CONTEXT_PTR->Chr < 0)
            {
                return;
            }
            if (CONTEXT_PTR->Chr == '#'  &&  (CONTEXT_PTR->Env.get(CONTEXT_PTR), CONTEXT_PTR->Chr == '{'))
            {
                ++n;
            }
            else if (CONTEXT_PTR->Chr == '}'  &&  (CONTEXT_PTR->Env.get(CONTEXT_PTR), CONTEXT_PTR->Chr == '#')  &&  --n < 0)
            {
                break;
            }
        }
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
    }
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
        while (*p++ = getByte(CONTEXT_PTR, &i, &w, &x));
    }
}

int skip(Context *CONTEXT_PTR)
{
    for (;;)
    {
        if (CONTEXT_PTR->Chr < 0)
        {
            return CONTEXT_PTR->Chr;
        }
        while (CONTEXT_PTR->Chr <= ' ')
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
            if (CONTEXT_PTR->Chr < 0)
            {
                return CONTEXT_PTR->Chr;
            }
        }

        if (CONTEXT_PTR->Chr != '#')
        {
            return CONTEXT_PTR->Chr;
        }
        comment(CONTEXT_PTR);
    }
}

#define AddFunc(M, N, F) \
    { \
    cdr(M) = addString(Mem, M, N);\
    setCARType(M, PTR_CELL);\
    setCARType(M->cdr, FUNC);\
    *(FunPtr *)(void*)&(car(cdr(M))) = F;/* This was necessary to satisfy the pedantic gcc Function pointer to object pointer */ \
    cdr(cdr(M)) = *Mem; \
    setCARType(cdr(cdr(M)), PTR_CELL);\
    M = cdr(M) + 1; \
    M = makeptr(M); \
    }

void printIndex(char *name, void *_MemStart, void *_Mem)
{
#if 0 // TODO - is this needed
    uword MemStart = (uword)_MemStart;
    uword Mem = (uword)_Mem;
    int index = (Mem - MemStart)/sizeof(cell);

    printf("Index of %s is %d\n", name, index);
#endif
}

#define MEM_SIZE 500
void setupBuiltinFunctions(any * Mem)
{

    *Mem = (any)calloc(sizeof(cell), MEM_SIZE);

    any memCell = *Mem, tempCell;

    car(memCell) = *Mem;
    cdr(memCell) = *Mem;
    setCARType(memCell, PTR_CELL);

    memCell++;
    cdr(memCell) = *Mem;
    setCARType(memCell, PTR_CELL);
    printIndex("Nil", *Mem, memCell);
    memCell = addString(Mem, memCell, "Nil");

    cdr(memCell) = *Mem;
    setCARType(memCell, PTR_CELL);
    printIndex("T", *Mem, memCell);
    memCell = addString(Mem, memCell, "T");

    cdr(memCell) = *Mem + 1;
    setCARType(memCell, PTR_CELL);
    printIndex("@", *Mem, memCell);
    memCell = addString(Mem, memCell, "@");

    cdr(memCell) = *Mem + 1;
    setCARType(memCell, PTR_CELL);
    printIndex("@@", *Mem, memCell);
    memCell = addString(Mem, memCell, "@@");

    cdr(memCell) = *Mem + 1;
    setCARType(memCell, PTR_CELL);
    printIndex("@@@", *Mem, memCell);
    memCell = addString(Mem, memCell, "@@@");

    AddFunc(memCell, "quote", doQuote);
    AddFunc(memCell, "de", doDe); 
    AddFunc(memCell, "bye", doBye);
    AddFunc(memCell, "+", doAdd);
    AddFunc(memCell, "-", doSub);
    AddFunc(memCell, "*", doMul);
    AddFunc(memCell, "/", doDiv);
    AddFunc(memCell, "%", doMod);
    AddFunc(memCell, ">>", doBinRShift);
    AddFunc(memCell, "!", doBinNot);
    AddFunc(memCell, "&", doBinAnd);
    AddFunc(memCell, "|", doBinOr);
    AddFunc(memCell, "x|", doBinXor);
    AddFunc(memCell, "**", doPow);
    AddFunc(memCell, ">", doNumGt);
    AddFunc(memCell, "<", doNumLt);
    AddFunc(memCell, "rand", doRandom);
    AddFunc(memCell, "let", doLet);
    AddFunc(memCell, "prinl", doPrin);
    AddFunc(memCell, "do", doDo);
    AddFunc(memCell, "setq", doSetq);
    AddFunc(memCell, "make", doMake);
    AddFunc(memCell, "index", doIndex);
    AddFunc(memCell, "link", doLink);
    AddFunc(memCell, "length", doLength);
    AddFunc(memCell, "list", doList);
    AddFunc(memCell, "cons", doCons);
    AddFunc(memCell, "car", doCar);
    AddFunc(memCell, "cdr", doCdr);
    AddFunc(memCell, "while", doWhile);
    AddFunc(memCell, "in", doIn);
    AddFunc(memCell, "out", doOut);
    AddFunc(memCell, "char", doChar);
    AddFunc(memCell, "sb", doSwitchBase);
    AddFunc(memCell, "line", doLine);
    AddFunc(memCell, "not", doNot);
    AddFunc(memCell, "for", doFor);
    AddFunc(memCell, "run", doRun);
    AddFunc(memCell, "hs", doHS);
    AddFunc(memCell, "and", doAnd);
    AddFunc(memCell, "or", doOr);
    AddFunc(memCell, "=", doEq);
    AddFunc(memCell, "if", doIf);
    AddFunc(memCell, "cond", doCond);
    AddFunc(memCell, "call", doCall);
    AddFunc(memCell, "pack", doPack);
    AddFunc(memCell, "load", doLoad);
    AddFunc(memCell, "eval", doEval);
    AddFunc(memCell, "mapcar", doMapcar);
    AddFunc(memCell, "loop", doLoop);
    AddFunc(memCell, "chop", doChop);
    AddFunc(memCell, "args", doArgs);
    AddFunc(memCell, "next", doNext);
    AddFunc(memCell, "thread", doThread);
    AddFunc(memCell, "sleep", doSleep);
    AddFunc(memCell, "rd", doRd);
    AddFunc(memCell, "wr", doWr);
    AddFunc(memCell, "++", doPopq);
    AddFunc(memCell, "inc", doInc);
    AddFunc(memCell, "dec", doDec);
    AddFunc(memCell, "bind", doBind);
    AddFunc(memCell, "listen", doListen);
    AddFunc(memCell, "socket", doSocket);
    AddFunc(memCell, "connect", doConnect);
    AddFunc(memCell, "tid", doTid);
    AddFunc(memCell, "cmp", doCmp);
    AddFunc(memCell, "os", doOs);
    AddFunc(memCell, "argv", doArgv);
    
    WORD_TYPE end = (WORD_TYPE)memCell;
    WORD_TYPE start = (WORD_TYPE)*Mem;
    MEMS = (end - start)/sizeof(cell);

    if (MEMS > MEM_SIZE)
    {
        printf("Not enough memory for builtin functions\n");
        exit(0);
    }
}

void addBuiltinFunction(any * Mem, char *fn, FunPtr fptr)
{
    any memCell = &((*Mem)[MEMS]);

    AddFunc(memCell, fn, fptr);
    
    WORD_TYPE end = (WORD_TYPE)memCell;
    WORD_TYPE start = (WORD_TYPE)*Mem;
    MEMS = (end - start)/sizeof(cell);

    if (MEMS > MEM_SIZE)
    {
        printf("Not enough memory for builtin functions\n");
        exit(0);
    }
}

any addString(any *Mem, any m, char *s)
{
    int ctr = 0;
    int shift = 0;
    setCARType(m, BIN_START);
    car(m) = m + 1;
    m++;
    while(*s)
    {
        setCARType(m, BIN);

        ((*(WORD_TYPE*)m))|=(((WORD_TYPE)*s)<<shift) ;
        shift += 8;
        if (++ctr == LISP_WORD_SIZE)
        {
            ctr=0;
            shift = 0;
            if (*(s+1))
            {
                cdr(m) = m + 1;
                setCARType(m, BIN);
                m++;
            }
        }
        s++;
    }

    cdr(m) = *Mem;//TODO
    setCARType(m, BIN);

    return m + 1;
}

void initialize_context(Context *CONTEXT_PTR)
{
   heapAlloc(CONTEXT_PTR);
   CONTEXT_PTR->Intern[0] = CONTEXT_PTR->Intern[1] = CONTEXT_PTR->Transient[0] = CONTEXT_PTR->Transient[1] = Nil;

   for (int i = 1; i < MEMS; i++)
   {
      any cell = (any)(CONTEXT_PTR->Mem + i);

      if (isSym(cell))
      {
         dump("symbol1");
         intern(CONTEXT_PTR, cell, CONTEXT_PTR->Intern);
         dump("symbol2");
      }
   }
}

any consIntern(Context *CONTEXT_PTR, any x, any y)
{
    dump("consIntern1");
    any r = cons(CONTEXT_PTR, x, y);
    dump("consIntern2");

    setCARType(r, PTR_CELL);
    dump("consIntern3");

    return r;
}

any consSym(Context *CONTEXT_PTR, any val, any w)
{
    cell *p;

    if (!(p = CONTEXT_PTR->Avail)) {
        cell c1;

        if (!val)
            gc(CONTEXT_PTR, CELLS);
        else {
            Push(c1,val);
            gc(CONTEXT_PTR, CELLS);
            drop(c1);
        }
        p = CONTEXT_PTR->Avail;
    }
    CONTEXT_PTR->Avail = car(p);
    cdr(p) = val ? val : p;
    car(p) = (any)w;
    return p;
}

int CONSCTR;
any cons(Context *CONTEXT_PTR, any x, any y)
{
    cell *p;

    CONSCTR++;
    dump("cons1");
    if (!(p = CONTEXT_PTR->Avail))
    {
        cell c1, c2;

        Push(c1,x);
        Push(c2,y);
        gc(CONTEXT_PTR, CELLS);
        drop(c1);
        p = CONTEXT_PTR->Avail;
        dump("cons2");
    }
    CONTEXT_PTR->Avail = car(p);
    car(p) = x;
    cdr(p) = y;
    setCARType(p, PTR_CELL);
    dump("cons3");

    return p;
}

any consName(Context *CONTEXT_PTR, uword w, any n)
{
   cell *p;

   if (!(p = CONTEXT_PTR->Avail))
   {
       cell c1;
       Push(c1, n);
      gc(CONTEXT_PTR, CELLS);
      drop(c1);
      p = CONTEXT_PTR->Avail;
   }
   CONTEXT_PTR->Avail = car(p);
   p = symPtr(p);
   car(p) = (any)w;
   cdr(p) = n;
   setCARType(p, PTR_CELL);
   return p;
}

uword length(Context *CONTEXT_PTR, any x)
{
   uword n;

   if (!isCell(x)) return 0;
   if (cdr(x) == x) return 0;

   for (n = 0; !isNil(x); x = cdr(x)) ++n;
   return n;
}

void varError(any ex, any x)
{
    err(ex, x, "Variable expected");
}

any undefined(Context *CONTEXT_PTR, any x, any ex)
{
    print(CONTEXT_PTR, x);
    printf(" is undefined\n");
    return Nil;
}

void makeError(any ex)
{
    err(ex, NULL, "Not making");
}

void numError(any ex, any x)
{
    err(ex, x, "Number expected");
}

void eofErr(void)
{
    err(NULL, NULL, "EOF Overrun");
}

void err(any ex, any x, char *fmt, ...)
{
    printf("ERROR\n");
    bye(0);
    if (ex == x) bye(1);
    if (fmt == NULL) bye(1);
}


void atomError(any ex, any x)
{
    err(ex, x, "Atom expected");
}

void lstError(any ex, any x)
{
    err(ex, x, "List expected");
}

void openErr(any ex, char *s)
{
    err(ex, NULL, "%s open: %s", s, strerror(errno));
}

int MEMS;
any Mem;

int PUSH_POP=0;
void ppp(Context*CONTEXT_PTR, char *m, cell c)
{
    //for (int i = 0; i < PUSH_POP; i++) printf(" ");
    //printf("c.car=%p c.cdr=%p Env->stack=%p %s", (c).car, (c).cdr, CONTEXT_PTR->Env.stack, m);
}

void debugIndent(Context *CONTEXT_PTR)
{
    CONTEXT_PTR->Tab++;
}

void debugOutdent(Context *CONTEXT_PTR)
{
    CONTEXT_PTR->Tab--;
}

void debugLog(Context *CONTEXT_PTR, char *message)
{
    for(int i = 0; i < CONTEXT_PTR->Tab; i++) fprintf(stderr, "  ");
    fprintf(stderr, "%s\n", message);
}

void debugLogAny(Context *CONTEXT_PTR, char *message, any x)
{
    for(int i = 0; i < CONTEXT_PTR->Tab; i++) fprintf(stderr, "  ");
    fprintf(stderr, "%s", message);
    print(CONTEXT_PTR, x);
    printf("\n");
}


void RestoreStack(Context *From, Context *To)
{
    any stackptr = From->Env.stack;
    if (!stackptr) return;
    To->Env.stack = (any)calloc(sizeof(cell), 1);
    any tostackptr = To->Env.stack;

    while (stackptr)
    {
        any fromCell = car(stackptr);
        any toCell = car(tostackptr) = (any)calloc(sizeof(cell), 1);

        uword *temp = (uword*)cdr(fromCell);
        any cdrOfFromCell = (any)temp[0];
        CellPartType type = fromCell->type;

        any c = car(fromCell);
        if (c)
        {
            uword *temp2 = (uword*)makeptr(c)->cdr;
            toCell->car = (any)temp2[1];
        }

        toCell->type = type;
        if (cdrOfFromCell != 0)
        {
            any x = makeptr(cdrOfFromCell);
            uword *temp2 = (uword*)x->cdr;
            toCell->cdr = (any)temp2[1];
        }


        stackptr = cdr(stackptr);
        if (stackptr)
        {
            cdr(tostackptr) = (any)calloc(sizeof(cell), 1);
            tostackptr = cdr(tostackptr);
        }
        else
        {
            cdr(tostackptr) = NULL;
        }
    }
}

void copyHeap(Context *From, Context *To)
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
        copyBackupCell(fromCell, toCell);
    }
    while(from)
    {
        for(int j=0; j < CELLS; j++)
        {
            cell *fromCell = &from->cells[j];
            cell *toCell = &to->cells[j];
            copyBackupCell(fromCell, toCell);
        }

        from=from->next;
        to=to->next;
    }

    dumpMemory(From, "t1");
    dumpMemory(To, "t1");

    /////////////////////////////////////////////////////
    from = From->Heaps;
    to = To->Heaps;
    for(int i = 0; i < MEMS; i++)
    {
        cell *fromCell = From->Mem + i;
        cell *toCell = To->Mem + i;
        copyFixupCell(From, To, fromCell, toCell);
    }
    while(from)
    {
        for(int j=0; j < CELLS; j++)
        {
            any fromCell = &from->cells[j];
            any toCell = &to->cells[j];
            copyFixupCell(From, To, fromCell, toCell);

        }

        from=from->next;
        to=to->next;
    }

    /////////////////////////////////////////////////////
    


    // COPY STACK
    RestoreStack(From, To);


    /////////////////////////////////////////////////////
    from = From->Heaps;
    to = To->Heaps;
    for(int i = 0; i < MEMS; i++)
    {
        cell *fromCell = From->Mem + i;
        cell *toCell = To->Mem + i;
        copyRestoreCell(From, To, fromCell, toCell);
    }
    while(from)
    {
        for(int j=0; j < CELLS; j++)
        {
            any fromCell = &from->cells[j];
            any toCell = &to->cells[j];
            copyRestoreCell(From, To, fromCell, toCell);
        }

        from=from->next;
        to=to->next;
    }

    //dumpMem(From, "DEBUG_HEAP2.txt");
    //dumpMem(To, "DEBUG_COPY2.txt");
}

void copyBackupCell(cell *fromCell, cell * toCell)
{
    uword  *temp = (uword*)calloc(sizeof(uword*) * 3, 1);
    temp[0] = (uword)fromCell->cdr;
    temp[1] = (uword)toCell;
    temp[2] = (uword)(fromCell->type);
    fromCell->cdr = (any)temp;
}

void copyFixupCell(Context *From, Context *To, cell *fromCell, cell * toCell)
{
    uword *temp = (uword*)fromCell->cdr;
    any cdrOfFromCell = (any)temp[0];
    CellPartType type = temp[2];

    if (type == EXT)
    {
        external *e = (external*)fromCell->car;
        if (e) toCell->car = (any)e->copy(From, e);
        else toCell->car = fromCell->car;
        toCell->type = EXT;
    }
    else if (!cdrOfFromCell)
    {
        toCell->car = NULL;
        toCell->type = type;
    }
    else if (type == FUNC || type == BIN)
    {
        toCell->car = fromCell->car;
        toCell->type = type;
    }
    else // PTR_CELL
    {
        uword *temp2 = (uword*)makeptr(fromCell->car)->cdr;
        toCell->car = (any)temp2[1];
        toCell->type = PTR_CELL;
    }

    if (cdrOfFromCell != 0)
    {
        any x = makeptr(cdrOfFromCell);
        uword *temp2 = (uword*)x->cdr;
        toCell->cdr = (any)temp2[1];
    }
}

void copyRestoreCell(Context *From, Context *To, cell *fromCell, cell *toCell)
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

    uword *temp = (uword*)fromCell->cdr;
    fromCell->cdr = (any)temp[0];
    free(temp);
}

extern int CONSCTR;

void *thread_func(void *arg)
{
    Context *CONTEXT_PTR = arg;

    dumpMemory(CONTEXT_PTR, "thIN");

    CONSCTR=1000;
    EVAL(CONTEXT_PTR, CONTEXT_PTR->Code);

    gc(CONTEXT_PTR, CELLS);

    heap *h = CONTEXT_PTR->Heaps;

    while (h)
    {
        heap *x = h;
        h = h->next;
        free(x);
    }

    free(CONTEXT_PTR->Mem);
    free(CONTEXT_PTR);
    //TODO - Free the Env->Stack

    return NULL;
}

any doThread(Context *CONTEXT_PTR_ORIG, any x)
{
    Context *CONTEXT_PTR = CONTEXT_PTR_ORIG;

    CONTEXT_PTR = (Context*)calloc(1, sizeof(Context));
    CONTEXT_PTR->InFile = stdin, CONTEXT_PTR->Env.get = getStdin;
    CONTEXT_PTR->OutFile = stdout, CONTEXT_PTR->Env.put = putStdout;
    CONTEXT_PTR->Code = caddr(x);
    CONTEXT_PTR_ORIG->Code = CONTEXT_PTR->Code;

    CONTEXT_PTR->ApplyArgs = Nil; //cons(CONTEXT_PTR, cons(CONTEXT_PTR, consSym(CONTEXT_PTR, Nil, 0), Nil), Nil);
    CONTEXT_PTR->ApplyBody = Nil; //cons(CONTEXT_PTR, Nil, Nil);
    CONTEXT_PTR->THREAD_COUNT = 1;
    CONTEXT_PTR->THREAD_ID = GetThreadID();

    dumpMemory(CONTEXT_PTR_ORIG, "th0");

    copyHeap(CONTEXT_PTR_ORIG, CONTEXT_PTR);

    dumpMemory(CONTEXT_PTR_ORIG, "th1");

    CONTEXT_PTR->Mem[0].car = CONTEXT_PTR->Mem[0].cdr; // TODO - should find a better place for this
    if (!CONTEXT_PTR_ORIG->Avail)
    {
        CONTEXT_PTR->Avail = 0;
    }
    else if (!CONTEXT_PTR_ORIG->Avail->car)
    {
        CONTEXT_PTR->Avail->car = 0;
    }

    dumpMemory(CONTEXT_PTR, "th2");


    // Clear out the items that need to be moved to the new thread
    x = cadr(x);
    any m = EVAL(CONTEXT_PTR_ORIG, car(x));
    do
    {
        if (GetType(m) == EXT)
        {
            car(m) = NULL;
        }
        x = cdr(x);
        m = EVAL(CONTEXT_PTR_ORIG, car(x));
    }
    while(x != (&(CONTEXT_PTR_ORIG->Mem[0])));

    plt_thread_start(CONTEXT_PTR, thread_func, 0); //TODO - passing nowait seems to not work

    CONTEXT_PTR = CONTEXT_PTR_ORIG;
    return Nil;
}

any doSleep(Context *CONTEXT_PTR, any ex)
{
    uword n;
    any x,y;
    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = mp_get_i32(num(y));

    plt_sleep(n);

    return y;
}

any doTid(Context *CONTEXT_PTR, any ex)
{
    return pltGetThreadId(CONTEXT_PTR);
}

any doBind(Context *CONTEXT_PTR, any ex)
{
    uword n;
    any x,y;
    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = mp_get_i32(num(y));

    return pltBind(CONTEXT_PTR, n);
}

any doConnect(Context *CONTEXT_PTR, any ex)
{
    return pltConnect(CONTEXT_PTR, ex);
}

any doListen(Context *CONTEXT_PTR, any ex)
{
    uword n;
    any x,y;
    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;

    external *e = (external*)car(y);
    n = (uword)e->pointer;

    return pltListen(CONTEXT_PTR, n);
}

any doSocket(Context *CONTEXT_PTR, any ex)
{
    return pltSocket(CONTEXT_PTR, ex);
}

void releaseSocket(struct _external* obj)
{
    pltClose(obj);
}

void releaseMalloc(external *ext)
{
    word *ptr = ext->pointer;
    void (*destructor)(external *) = *ptr;
    destructor(ext);
    free(ext->pointer);
    free(ext);
}

char * printMalloc(Context *CONTEXT_PTR, external* ext)
{
    char *mem=(char*)malloc(7);
    strcpy(mem, "MALLOC");
    return mem;
}

int equalMalloc(Context *CONTEXT_PTR, external *ext1, external *ext2)
{
    return 0;
}

external *copyMalloc(Context *CONTEXT_PTR, external *ext)
{
    return ext;
}

external *allocateMemBlock(Context *CONTEXT_PTR, word size, void (*destructor)(external*))
{
    external *ptr = (external *)malloc(sizeof(external));
    ptr->type = EXT_MALLOC;
    ptr->release = releaseMalloc;
    ptr->print = printMalloc;
    ptr->equal = equalMalloc;
    ptr->copy = copyMalloc;
    ptr->pointer = (void*)malloc(size);

    word *dest = ptr->pointer;
    *dest = (word)destructor;

    return ptr;
}

external * copySocket(Context *CONTEXT_PTR, external *ext)
{
    return ext;
}

int equalSocket(Context *CONTEXT_PTR, external*x, external*y)
{
    if (x->type != EXT_SOCKET)
    {
        fprintf(stderr, "LHS is not socket\n");
        return 1;
    }

    if (y->type != EXT_SOCKET)
    {
        fprintf(stderr, "RHS is not socket\n");
        return 1;
    }

    return x == y;
}

char * printSocket(Context *CONTEXT_PTR, struct _external* obj)
{
    char *buf=(char *)malloc(256);
    sprintf(buf, "Socket %p\n", obj->pointer);
    return buf;
}

