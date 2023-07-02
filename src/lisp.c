#include "lisp.h"

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
    else if (isSym(x))
    {
        return val(x);
    }
    else if (isFunc(x))
    {
        giveup("Unexpected Func");
    }
    else if (isNum(x))
    {
        giveup("Unexpected Num");
    }
    else
    {
        return evList(CONTEXT_PTR, x);
    }
}

any evExpr(Context *CONTEXT_PTR, any expr, any x)
{
   any y = car(expr);
   uword len = length(CONTEXT_PTR, y);

   bindFrame *f = allocFrame(len + 2);

   f->link = CONTEXT_PTR->Env.bind,  CONTEXT_PTR->Env.bind = f;
   f->i = (bindSize * (len + 2)) / (2*sizeof(any)) - 1;
   f->cnt = 1,  f->bnd[0].sym = At,  f->bnd[0].val = val(At);

   while (!isNil(y) && isCell(y))
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

    if (isSym(foo) && cdr(foo) == foo)
    {
        return ex;
    }

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

any isIntern(Context *CONTEXT_PTR, any nm, any tree)
{
    any x, y, z;
    word n;

    for (x = tree; !isNil(x);)
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

int getByte1(Context *CONTEXT_PTR, int *i, uword *p, any *sym)
{
    if (isSym(*sym))
    {
        (*sym)=car(*sym);
        *i = BITS;
        *p = (uword)(car(*sym));
        *sym = (cdr(*sym));
    }
    else
    {
        giveup("Cant getByte");
    }

    int c = *p & 0xff;
    *p >>= 8;
    *i -= 8;
    return c;
}

int getByte(Context *CONTEXT_PTR, int *i, uword *p, any *q)
{
    if (*i == 0)
    {
        if (isNil(*q))
        {
            return 0;
        }

        *i = BITS;
        *p = (uword)(car(*q));
        *q = cdr(*q);
    }

    int c = *p & 0xff;
    *p >>= 8;
    *i -= 8;
    return c;
}

int isSymAtNode(Context *CONTEXT_PTR, any sym, any node, word *n)
{
    any y = car(sym);
    any z = car(car(node));

    while ((*n = (word)(car(y)) - (word)car(z)) == 0)
    {
        if (GetType(y) != BIN)
        {
            return 1;
        }
        y = cdr(y);
        z = cdr(z);
    }

    return 0;
}

int addSymToLeftOfNode(Context *CONTEXT_PTR, any sym, any *node)
{
    if (isNil(cdr(*node)))
    {
        any newNode = cons(CONTEXT_PTR, sym, Nil);
        newNode = cons(CONTEXT_PTR, newNode, Nil);
        cdr(*node) = newNode;
        return 1;
    }

    *node = cdr(*node);

    if (isNil(car(*node)))
    {
        car(*node) = cons(CONTEXT_PTR, sym, Nil);
        return 1;
    }

    *node = car(*node);
    return 0;
}

int addSymToRightOfNode(Context *CONTEXT_PTR, any sym, any *node)
{
    if (isNil(cdr(*node)))
    {
        any newNode = cons(CONTEXT_PTR, sym, Nil);
        newNode = cons(CONTEXT_PTR, Nil, newNode);
        cdr(*node) = newNode;
        return 1;
    }

    *node = cdr(*node);

    if (isNil(cdr(*node)))
    {
        cdr(*node) = cons(CONTEXT_PTR, sym, Nil);
        return 1;
    }

    *node = cdr(*node);
    return 0;
}

any intern(Context *CONTEXT_PTR, any sym, any *root)
{
    any node = *root;

    if (isNil(node))
    {
        *root = cons(CONTEXT_PTR, sym, Nil);
        return *root;
    }

    for (;;)
    {
        word n;
        if (isSymAtNode(CONTEXT_PTR, sym, node, &n))
        {
            return sym;
        }

        if (n < 0)
        {
            if (addSymToLeftOfNode(CONTEXT_PTR, sym, &node))
            {
                return sym;
            }
        }
        else
        {
            if (addSymToRightOfNode(CONTEXT_PTR, sym, &node))
            {
                return sym;
            }
        }
    }
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

    struct bn *BIGNUM = (struct bn*)malloc(sizeof(struct bn));
    bignum_from_string_fixed(BIGNUM, str, LEN, realloc);
    free(str);
    NewNumber(BIGNUM, r);
    return r;

returnNULL:
    free(str);
    return NULL;

}

any mkChar(Context *CONTEXT_PTR, int c)
{
	any x;
	byte b[2];
	b[0] = (byte)c;
	b[1] = 0;
	any y = mkSym(CONTEXT_PTR, (byte*)&b);

	// intern into transient to make " appear

	if (x = isIntern(CONTEXT_PTR, tail(y), CONTEXT_PTR->Transient))
	{
		return x;
	}

	intern(CONTEXT_PTR, y, &CONTEXT_PTR->Transient);
	return y;
}



any putSymByte(Context *CONTEXT_PTR, any curCell, int *shift, byte b)
{
    uword *ptr = (uword *)&(curCell->car);

    if (*shift == BITS)
    {
        *shift = 0;
        curCell->cdr = cons(CONTEXT_PTR, Nil, Nil);
        curCell = curCell->cdr;
        setCARType(curCell, BIN);
        ptr = (uword *)&(curCell->car);
        *ptr = 0;
    }

    *ptr |= (uword)b << (*shift);
    *shift += 8;

    return curCell;
}

any startSym(Context *CONTEXT_PTR, any c1)
{
    any q = cons(CONTEXT_PTR, Nil, Nil);
    Push(*c1, q);

    q->car = cons(CONTEXT_PTR, Nil, Nil);
    q->cdr = q;
    q->type = PTR_CELL;

    any curCell = q->car;
    curCell->type = BIN;

    uword *ptr = (uword *)&(curCell->car);
    *ptr=0;

    return curCell;
}

any mkSym(Context *CONTEXT_PTR, byte *s)
{
    cell c1;
    byte b;
    int shift = 0;
    any curCell = startSym(CONTEXT_PTR, &c1);

    while (b = *s++)
    {
        curCell = putSymByte(CONTEXT_PTR, curCell, &shift, b);
    }

    return Pop(c1);
}

int firstByte(Context*CONTEXT_PTR, any s)
{
    int i;
    uword w;
    return getByte1(CONTEXT_PTR, &i, &w, &s);
}

static int INDEX;

extern int CONSCTR;

char *ExtTypeString(any cell, char*buf)
{
    external *e = (external *)car(cell);
    int len;
    char *b;

    if (!e) return "NULL";
    switch(e->type)
    {
        case EXT_NUM:
            b = mpz_get_str(NULL, 10, (MP_INT*)num(cell));
            len = strlen(b);
            sprintf(buf, "%s", b);
            free(b);
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
   setMark(CONTEXT_PTR->Intern, 0);mark(CONTEXT_PTR, CONTEXT_PTR->Intern);
   setMark(CONTEXT_PTR->Intern, 0);mark(CONTEXT_PTR, CONTEXT_PTR->Intern);
   setMark(CONTEXT_PTR->Transient, 0);mark(CONTEXT_PTR, CONTEXT_PTR->Transient);
   setMark(CONTEXT_PTR->Transient, 0);mark(CONTEXT_PTR, CONTEXT_PTR->Transient);
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

    if (isSym(v) || isSym(v2))
    {
        if (!(isSym(v) && isSym(v2)))
        {
            return 1;
        }
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

any pack(Context *CONTEXT_PTR, any r, any x, int* shift, int *nonzero)
{
    if (!isNil(x) && isCell(x))
    {
        do
        {
            if (GetType(x) == PTR_CELL)
            {
                r = pack(CONTEXT_PTR, r, car(x), shift, nonzero);
            }
            else
            {
                r = pack(CONTEXT_PTR, r, x, shift, nonzero);
            }
        } while (!isNil(x = cdr(x)));
    
        return r;
    }

    if (isNil(x))
    {
        return r;
    }

    if (isNum(x))
    {
        char *buf = mpz_get_str(NULL, 10, num(x));
        char *b = buf;
        any curCell = r;
        uword *ptr = (uword *)&(curCell->car);

        do
        {
            *nonzero = 1;
            r = curCell = putSymByte(CONTEXT_PTR, curCell, shift, *b++);
        } while (*b);
        free(buf);

        return r;
    }
    else if (!isNil(x))
    {
        int c, j;
        uword w;
        any curCell = r;
        uword *ptr = (uword *)&(curCell->car);
        for (c = getByte1(CONTEXT_PTR, &j, &w, &x); c; c = getByte(CONTEXT_PTR, &j, &w, &x))
        {
            *nonzero = 1;
            r = curCell = putSymByte(CONTEXT_PTR, curCell, shift, c);
        }

        return r;
    }
}

// (pack 'any ..) -> sym
any doPack(Context *CONTEXT_PTR, any x)
{
    int nonzero = 0;
    int shift = 0;

    cell c1;
    any curCell = startSym(CONTEXT_PTR, &c1);

    cell c2;
    Push(c2, Nil);

    while (!isNil(x = cdr(x)))
    {
        data(c2) = EVAL(CONTEXT_PTR, car(x));
        curCell = pack(CONTEXT_PTR, curCell, data(c2), &shift, &nonzero);
    }

    if (nonzero)
    {
        return Pop(c1);
    }
    else
    {
        drop(c1);
        return Nil;
    }
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
    mpz_set_ui(BIGNUM, n);
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
                if (! mpz_cmp(num(f->bnd[0].sym->cdr), num(data(c1))))
                    break;

                MP_INT *n = (MP_INT*)malloc(sizeof(MP_INT));
                mpz_init(n);

                mpz_set(n, num(f->bnd[0].sym->cdr));
                mpz_add_ui(n, n, 1);

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

    x = cdr(ex);
    if (isNil(data(c1) = EVAL(CONTEXT_PTR, car(x))))
    {
        MP_INT *id = (MP_INT*)malloc(sizeof(MP_INT));
        mpz_init(id);
        mpz_set_ui(id, 0);

        NewNumber( id, idr);
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
        mpz_sub(n, n, m);

        drop(c2);
    }

    return Pop(c1);
}

// (/ 'num ..) -> num
any doDiv(Context *CONTEXT_PTR, any ex)
{
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
    MP_INT *c = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(c);
    MP_INT *d = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(d);
    mpz_divmod(c, d, num(data(c1)), num(data(c2)));

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

    x = cdr(ex);
    if (isNil(data(c1) = EVAL(CONTEXT_PTR, car(x))))
    {
        MP_INT *id = (MP_INT*)malloc(sizeof(MP_INT));
        mpz_init(id);
        mpz_set_ui(id, 1);
        NewNumber( id, idr);
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
        mpz_mul(n, m, n);

        drop(c2);
    }

    return Pop(c1);
}

// (** 'num ..) -> num
any doPow(Context *CONTEXT_PTR, any ex)
{
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

        int m = mpz_get_ui(num(y));
        mpz_pow_ui(n, n, m);

    }

    NewNumber( n, r);
    return r;
}

// (% 'num ..) -> num
any doMod(Context *CONTEXT_PTR, any ex)
{
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
    MP_INT *c = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(c);
    mpz_mod(c, num(data(c1)), num(data(c2)));

    NewNumber( c, r1);
    data(c1) = r1;

    return Pop(c1);
}

// (& 'num ..) -> num
any doBinNot(Context *CONTEXT_PTR, any ex)
{
    // TODO REVISIT
    return Nil;
}

// (& 'num ..) -> num
any doBinAnd(Context *CONTEXT_PTR, any ex)
{
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
        mpz_and(n, m, n);

    }

    NewNumber( n, r);
    return r;
}

// (| 'num ..) -> num
any doBinOr(Context *CONTEXT_PTR, any ex)
{
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
        mpz_or(n, m, n);

    }

    NewNumber( n, r);
    return r;
}

// (x| 'num ..) -> num
any doBinXor(Context *CONTEXT_PTR, any ex)
{
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
        mpz_xor(n, m, n);

    }

    NewNumber( n, r);
    return r;
}

// (>> 'num ..) -> num
any doBinRShift(Context *CONTEXT_PTR, any ex)
{
    any x, y;
    word s = 1;

    x = cdr(ex);
    any p1 = EVAL(CONTEXT_PTR, car(x));

    if (isNil(p1) || !isNum(p1)) return Nil;

    s = mpz_get_si(num(p1));

    x = cdr(x);
    any p2 = EVAL(CONTEXT_PTR, car(x));

    if (isNil(p2) || !isNum(p2)) return Nil;

    MP_INT *m = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(m);
    if (s >= 0)
    {
        mpz_div_2exp(m, num(p2), s);
    }
    else
    {
        s *= -1;
        mpz_mul_2exp(m, num(p2), s);
    }

    NewNumber( m, r);

    return r;
}

// (< 'num ..) -> num
any doNumLt(Context *CONTEXT_PTR, any ex)
{
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
        if (mpz_cmp(n, m) >= 0)
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
any doNumGt(Context *CONTEXT_PTR, any ex)
{
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

    mpz_clear((MP_INT*)p->pointer);
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


// (split 'lst 'any ..) -> lst
any doSplit(Context *CONTEXT_PTR, any x) {
   any y;
   int i;
   cell c1, res, sub;

   int n = length(CONTEXT_PTR, cdr(x=cdr(x)));

   if (!isCell(data(c1) = EVAL(CONTEXT_PTR, car(x))))
   {
      return data(c1);
   }

   cell *c = (cell *)calloc(sizeof(cell), n);

   Save(c1);
   for (i = 0; i < n; ++i)
      x = cdr(x),  Push(c[i], EVAL(CONTEXT_PTR, car(x)));
   Push(res, x = Nil);
   Push(sub, y = Nil);
   do {
      for (i = 0;  i < n;  ++i) {
         if (0 == equal(CONTEXT_PTR, car(data(c1)), data(c[i]))) {
            if (isNil(x))
               x = data(res) = cons(CONTEXT_PTR, data(sub), Nil);
            else
               x = cdr(x) = cons(CONTEXT_PTR, data(sub), Nil);
            y = data(sub) = Nil;
            goto spl1;
         }
      }
      if (isNil(y))
         y = data(sub) = cons(CONTEXT_PTR, car(data(c1)), Nil);
      else
         y = cdr(y) = cons(CONTEXT_PTR, car(data(c1)), Nil);
spl1: ;
   } while (!isNil(data(c1) = cdr(data(c1))));


   y = cons(CONTEXT_PTR, data(sub), Nil);
   drop(c1);
   free(c);

   if (isNil(x))
      return y;
   cdr(x) = y;
   return data(res);
}

// (mapcar 'fun 'lst ..) -> lst
any doMapcar(Context *CONTEXT_PTR, any ex)
{
   any x = cdr(ex);
   cell res, foo;

   Push(res, Nil);
   Push(foo, EVAL(CONTEXT_PTR, car(x)));

   x = cdr(x);
   if (!isNil(x) && isCell(x)) {
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

// (conc 'lst ..) -> lst
any doConc(Context *CONTEXT_PTR, any x) {
   any y, z;
   cell c1;

   x = cdr(x),  Push(c1, y = EVAL(CONTEXT_PTR, car(x)));
   while (!isNil(x = cdr(x))) {
      z = EVAL(CONTEXT_PTR, car(x));
      if (!isCell(y))
         y = data(c1) = z;
      else {
         while (!isNil(cdr(y)))
            y = cdr(y);
         cdr(y) = z;
      }
   }
   return Pop(c1);
}

// (pack 'any ..) -> sym
any doGlue(Context *CONTEXT_PTR, any x)
{
    int nonzero = 0;
    int shift = 0;

    cell c1, c2, c3, c4;
    any curCell = startSym(CONTEXT_PTR, &c1);

    x = cdr(x),  Push(c3, EVAL(CONTEXT_PTR, car(x)));
    x = cdr(x),  Push(c4, x =EVAL(CONTEXT_PTR, car(x)));

    if (!isCell(x))
    {
	    // Not a list
	    drop(c1);
	    return x;
    }

    if (!isNil(car(x)))
    {
        curCell = pack(CONTEXT_PTR, curCell, car(x), &shift, &nonzero);
    }

    while (!isNil(x = cdr(x)))
    {
        curCell = pack(CONTEXT_PTR, curCell, data(c3), &shift, &nonzero);
        curCell = pack(CONTEXT_PTR, curCell, car(x), &shift, &nonzero);
    }

    if (nonzero)
    {
        return Pop(c1);
    }
    else
    {
        drop(c1);
        return Nil;
    }
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

// (filter 'fun 'lst ..) -> lst
any doFilter(Context *CONTEXT_PTR, any ex)
{
   any x = cdr(ex);
   cell res, foo;

   Push(res, Nil);
   Push(foo, EVAL(CONTEXT_PTR, car(x)));
   
   x = cdr(x);
   if (!isNil(x) && isCell(x))
   {
      int i, n = 0;

      cell *c = (cell *)calloc(sizeof(cell), length(CONTEXT_PTR, x));

      do
         Push(c[n], EVAL(CONTEXT_PTR, car(x))), ++n;
      while (!isNil(x = cdr(x)));
      if (!isCell(data(c[0])))
      {
        free(c);
        return Pop(res);
      }

      while (isNil(apply(CONTEXT_PTR, ex, data(foo), YES, n, c))) {
         if (isNil(data(c[0]) = cdr(data(c[0]))))
         {
            free(c);
            return Pop(res);
         }
         for (i = 1; i < n; ++i)
            data(c[i]) = cdr(data(c[i]));
      }

      data(res) = x = cons(CONTEXT_PTR, car(data(c[0])), Nil);

      while (!isNil(data(c[0]) = cdr(data(c[0]))))
      {
         for (i = 1; i < n; ++i)
         {
            data(c[i]) = cdr(data(c[i]));
         }

         if (!isNil(apply(CONTEXT_PTR, ex, data(foo), YES, n, c)))
         {
            x = cdr(x) = cons(CONTEXT_PTR, car(data(c[0])), Nil);
         }
      }

      free(c);
   }

   return Pop(res);
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
      caar(x) = cf? car(data(p[n])) : data(p[n]);
      while (--n >= 0) {
         if (isNil(cdr(x)))
         {
            cdr(x) = cons(CONTEXT_PTR, cons(CONTEXT_PTR, consSym(CONTEXT_PTR, Nil, Nil), car(x)), Nil);
            setCARType(cdr(x), PTR_CELL);
         }
         x = cdr(x);
         caar(x) = cf? car(data(p[n])) : data(p[n]);
         setCARType(cdr(x), PTR_CELL);
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
   if (!CONTEXT_PTR->Chr)
   {
      CONTEXT_PTR->Env.get(CONTEXT_PTR);
   }

   if (eol(CONTEXT_PTR))
   {
      return Nil;
   }

   x = cdr(x);
   if (isNil(EVAL(CONTEXT_PTR, car(x))))
   {
      cell c1;
      any y = Nil;

      do
      {
            any c = mkChar(CONTEXT_PTR, CONTEXT_PTR->Chr);
            if (y == Nil)
            {
                Push(c1, cons(CONTEXT_PTR, c, Nil));
                y = data(c1);
            }
            else
            {
                cdr(y) = cons(CONTEXT_PTR, c, Nil);
                setCARType(y, PTR_CELL);
                y = cdr(y);
            }

            CONTEXT_PTR->Env.get(CONTEXT_PTR);

      } while (!eol(CONTEXT_PTR));

      return Pop(c1);
   }
   else
   {
      cell c1;
      any curCell = startSym(CONTEXT_PTR, &c1);
      int shift = 0;

      do
      {
         curCell = putSymByte(CONTEXT_PTR, curCell, &shift, CONTEXT_PTR->Chr);
         CONTEXT_PTR->Env.get(CONTEXT_PTR);
      } while (!eol(CONTEXT_PTR));

      return Pop(c1);
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
        return mkChar(CONTEXT_PTR, mpz_get_ui(num(x)));
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


    int order = 0;
    p2 = EVAL(CONTEXT_PTR, p2);
    if (isNum(p2))
    {
        order = mpz_get_ui(num(p2));
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
    any params = cdr(ex);
    any p1 = car(params);
    any p2 = cadr(params);
    any p3 = caddr(params);

    p1 = EVAL(CONTEXT_PTR, p1);
    if (!isNum(p1)) return Nil;
    

    char *buf = (char*)malloc(1024);
    int count = 0;
    int bufSize=1024;


    MP_INT temp, twofivefive, data;
    mpz_init(&temp);
    mpz_init(&data);
    mpz_init(&twofivefive);
    mpz_set_ui(&twofivefive, 255);
    mpz_set(&data, num(p1));

    if(!mpz_cmp_ui(&data, 0))
    {
        CONTEXT_PTR->Env.put(CONTEXT_PTR, 0);
        MP_INT *n = (MP_INT*)malloc(sizeof(MP_INT));
        mpz_init(n);
        mpz_set_ui(n, 1);

        NewNumber( n, r);
        return r;
    }

    while(mpz_cmp_ui(&data, 0))
    {
        mpz_and(&temp, &data, &twofivefive);
        buf[count++] = mpz_get_ui(&temp);
        mpz_div_2exp(&data, &data, 8);

        if (count == bufSize)
        {
            bufSize *= 2;
            buf=(char*)realloc(buf, bufSize);
        }
    }

    int order = 0;
    p2 = EVAL(CONTEXT_PTR, p2);
    if (isNum(p2))
    {
        order = mpz_get_ui(num(p2));
    }

    for (int i = 0; i < count; i++)
    {
        CONTEXT_PTR->Env.put(CONTEXT_PTR, buf[i]);
    }
    free(buf);


    MP_INT *n = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(n);
    mpz_set_ui(n, count);

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

    MP_INT *n = (MP_INT*)malloc(sizeof(MP_INT));
    mpz_init(n);
    mpz_set_ui(n, ret);

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
    char *buf = mpz_get_str(NULL, 10, num(n));
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

        int shift = 0;
        any curCell = startSym(CONTEXT_PTR, &c1);
        curCell = putSymByte(CONTEXT_PTR, curCell, &shift, CONTEXT_PTR->Chr);

        while (CONTEXT_PTR->Env.get(CONTEXT_PTR), CONTEXT_PTR->Chr != '"')
        {
            if (!testEsc(CONTEXT_PTR))
            {
                eofErr();
            }

            curCell = putSymByte(CONTEXT_PTR, curCell, &shift, CONTEXT_PTR->Chr);
        }

        y = Pop(c1);
        CONTEXT_PTR->Env.get(CONTEXT_PTR);

        if (x = isIntern(CONTEXT_PTR, tail(y), CONTEXT_PTR->Transient))
        {
            return x;
        }

        intern(CONTEXT_PTR, y, &CONTEXT_PTR->Transient);
        return y;
    }

    if (strchr(Delim, CONTEXT_PTR->Chr))
    {
        err(NULL, NULL, "Bad input '%c' (%d)", isprint(CONTEXT_PTR->Chr)? CONTEXT_PTR->Chr:'?', CONTEXT_PTR->Chr);
    }

    if (CONTEXT_PTR->Chr == '\\')
    {
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
    }

    int shift = 0;
    any curCell = startSym(CONTEXT_PTR, &c1);
    curCell = putSymByte(CONTEXT_PTR, curCell, &shift, CONTEXT_PTR->Chr);

    int count=0;
    for (;;)
    {
        count++;
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        if (strchr(Delim, CONTEXT_PTR->Chr))
        {
            break;
        }
        if (CONTEXT_PTR->Chr == '\\')
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
        }
        curCell = putSymByte(CONTEXT_PTR, curCell, &shift, CONTEXT_PTR->Chr);
    }

    dump("putbyte1");
    y = Pop(c1);

    dump("putbyte2");
    if (x = symToNum(CONTEXT_PTR, tail(y), 0, '.', 0))
    {
        return x;
    }
    if (x = isIntern(CONTEXT_PTR, tail(y), CONTEXT_PTR->Intern))
    {
        return x;
    }

    intern(CONTEXT_PTR, y, &CONTEXT_PTR->Intern);
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
    AddFunc(memCell, "glue", doGlue);
    AddFunc(memCell, "split", doSplit);
    AddFunc(memCell, "filter", doFilter);
    AddFunc(memCell, "conc", doConc);
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
    AddFunc(memCell, "rd", doRd);
    AddFunc(memCell, "wr", doWr);
    AddFunc(memCell, "++", doPopq);
    AddFunc(memCell, "inc", doInc);
    AddFunc(memCell, "dec", doDec);
    AddFunc(memCell, "cmp", doCmp);
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
   CONTEXT_PTR->Intern = CONTEXT_PTR->Transient = Nil;

   for (int i = 1; i < MEMS; i++)
   {
      any cell = (any)(CONTEXT_PTR->Mem + i);

      if (isSym(cell))
      {
         dump("symbol1");
         intern(CONTEXT_PTR, cell, &CONTEXT_PTR->Intern);
         dump("symbol2");
      }
   }
}

any consSym(Context *CONTEXT_PTR, any val, any w)
{
    any p = cons(CONTEXT_PTR, (any)w, val);
    if (!val) cdr(p) = p;
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

void ppp(Context*CONTEXT_PTR, char *m, cell c)
{
    //for (int i = 0; i < PUSH_POP; i++) printf(" ");
    //printf("c.car=%p c.cdr=%p Env->stack=%p %s", (c).car, (c).cdr, CONTEXT_PTR->Env.stack, m);
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
