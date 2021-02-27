#include "lisp.h"
#include "cell.h"

/*** Macros ***/
#define Free(p)         ((p)->car=_CONTEXT_PTR->Avail, (p)->cdr=0, (p)->type._t=0,  _CONTEXT_PTR->Avail=(p))


#if INTPTR_MAX == INT32_MAX
    #include "def32.d"
#elif INTPTR_MAX == INT64_MAX
    #include "def64.d"
#else
    #error "Unsupported bit width"
#endif

static void mark(any x);
static void mark(any x)
{
    if (!x) return;

    if (getMark(x)) return;

    setMark(x, 1);

    if (x == Nil) return;

    if (getCARType(x) == BIN_START)
    {
        if (getCDRType(x) == PTR_CELL) mark(cdr(x));
        x = x->car;
        while(x && x != Nil)
        {
            mark(x);
            x=x->cdr;
        }
        return;
    }


    if (getCARType(x) == PTR_CELL || getCARType(x) == INTERN) mark(car(x));

    while (1)
    {
        if (getCDRType(x) != PTR_CELL && getCARType(x) != INTERN) break;
        x = cdr(x);
        if (!x) break;
        if (x==Nil) break;
        if (getMark(x)) break;
        setMark(x, 1);
        if (getCARType(x) == BIN_START)
        {
            setMark(x, 0);
            mark(x);
        }
        if (getCARType(x) == PTR_CELL || getCARType(x) == INTERN) mark(car(x));
    }
}

void dump(FILE *fp, any p)
{

    if (getCARType(p) == TXT)
    {
        fprintf(fp, "%p %s(TXT = %p) %p %p\n", p, (char*)&(p->car),p->car, p->cdr, (void*)p->type._t);
    }
    else
    {
        fprintf(fp, "%p ", p);
        if(p->car) fprintf(fp, "%p ", p->car); else fprintf(fp, "0 ");
        if(p->cdr) fprintf(fp, "%p ", p->cdr); else fprintf(fp, "0 ");
        if(p->type._t) fprintf(fp, "%p\n", (void *)p->type._t); else fprintf(fp, "0\n");
    }
}

void sweep(int free)
{
   any p;
   heap *h;
   int c =100;
   /* Sweep */
   if(free)_CONTEXT_PTR->Avail = NULL;
   h = _CONTEXT_PTR->Heaps;
   if (c) {
      do {
         p = h->cells + CELLS-1;
         do
         {
            if (!getMark(p))
            {
                printf("FREEING %p  .... \n", p);
                if (free) Free(p);
               --c;
            }
            if(free)setMark(p, 0);
         }
         while (--p >= h->cells);
      } while (h = h->next);

      //while (c >= 0)
      //{
      //   heapAlloc(),  c -= CELLS;
      //}
   }

   printf("AVAIL = %p\n", _CONTEXT_PTR->Avail);
}


void dumpHeaps(FILE *mem, heap *h)
{
    any p;
    if (!h) return;
    dumpHeaps(mem, h->next);

    fprintf(mem, "# START HEAP\n");
    p = h->cells + CELLS-1;
    do
    {
        //fprintf(mem, "0x%016lx %p %p %p\n", p, p->car, p->cdr, p->type._t);
        dump(mem, p);
    }
    while (--p >= h->cells);
}

void markAll(void);
void markAll()
{
   any p;
   int i;

   for (i = 0; i < MEMS; i += 3)
   {
       setMark((any)&Mem[i], 0);
       mark((any)&Mem[i]);
   }

   /* Mark */
   setMark(_CONTEXT_PTR->Intern[0], 0);mark(_CONTEXT_PTR->Intern[0]);
   setMark(_CONTEXT_PTR->Intern[1], 0);mark(_CONTEXT_PTR->Intern[1]);
   setMark(_CONTEXT_PTR->Transient[0], 0);mark(_CONTEXT_PTR->Transient[0]);
   setMark(_CONTEXT_PTR->Transient[1], 0);mark(_CONTEXT_PTR->Transient[1]);
   if (_CONTEXT_PTR->ApplyArgs) setMark(_CONTEXT_PTR->ApplyArgs, 0);mark(_CONTEXT_PTR->ApplyArgs);
   if (_CONTEXT_PTR->ApplyBody) setMark(_CONTEXT_PTR->ApplyBody, 0);mark(_CONTEXT_PTR->ApplyBody);
   for (p = _CONTEXT_PTR->Env.stack; p; p = cdr(p))
   {
      mark(car(p));
   }
   for (p = (any)_CONTEXT_PTR->Env.bind;  p;  p = (any)((bindFrame*)p)->link)
   {
      for (i = ((bindFrame*)p)->cnt;  --i >= 0;)
      {
         mark(((bindFrame*)p)->bnd[i].sym);
         mark(((bindFrame*)p)->bnd[i].val);
      }
   }
   for (p = (any)_CONTEXT_PTR->CatchPtr; p; p = (any)((catchFrame*)p)->link)
   {
      if (((catchFrame*)p)->tag)
         mark(((catchFrame*)p)->tag);
      mark(((catchFrame*)p)->fin);
   }
}

any doHS(Context *CONTEXT_PTR, any ignore)
{
    getHeapSize();
    return ignore;
}

any doDump(Context *CONTEXT_PTR, any ignore)
{
    static int COUNT=0;
    char debugFileName[100];
    sprintf(debugFileName, "debug-%03d.mem", COUNT++);
    // if (T == cadr(ignore))
    // {
    //     markAll();
    //     sweep(0);
    // }
    // if ( 0 == car(cadr(ignore)))
    // {
    //     markAll();
    //     sweep(1);
    // }

    // if ( 0 == car(cadr(ignore)))
    // {
    //     gc(_CONTEXT_PTR, CELLS);
    // }

    if (Nil == ignore)
    {
        return ignore;
    }

    FILE *mem;
    mem = fopen(debugFileName, "w");

    fprintf(mem, "# START MEM\n");
    for (int i = 0; i < MEMS; i += 3)
    {
        //fprintf(mem, "0x%016lx %p %p %p\n", &Mem[i], Mem[i], Mem[i + 1], Mem[i + 2]);
        dump(mem, (any)(&Mem[i]));
    }

    heap *h = _CONTEXT_PTR->Heaps;

    dumpHeaps(mem, h);

    fclose(mem);

    return Nil;
}


uword getHeapSize(void)
{
    uword size = 0;
    uword sizeFree = 0;
    heap *h = _CONTEXT_PTR->Heaps;
    do
    {
        any p = h->cells + CELLS-1;
        do
        {
            size++;
        }
        while (--p >= h->cells);
    } while (h = h->next);

    any p = _CONTEXT_PTR->Avail;
    while (p)
    {
        sizeFree++;
        p = car(p);
    }

    printf("MEM SIZE = " WORD_FORMAT_STRING_D " FREE = " WORD_FORMAT_STRING_D "\n", size, sizeFree);

    return size;
}

/* Garbage collector */
static void gc(Context *CONTEXT_PTR, word c)
{
    any p;
    heap *h;

    doDump(CONTEXT_PTR, Nil);
    markAll();
    doDump(CONTEXT_PTR, Nil);

    /* Sweep */
    _CONTEXT_PTR->Avail = NULL;
    h = _CONTEXT_PTR->Heaps;
    if (c)
    {
        do
        {
            p = h->cells + CELLS-1;
            do
            {
                if (!getMark(p))
                {
                    Free(p);
                    --c;
                }
                setMark(p, 0);
            }
            while (--p >= h->cells);
        } while (h = h->next);


        while (c >= 0)
        {
            heapAlloc(),  c -= CELLS;
        }
    }

    doDump(CONTEXT_PTR, Nil);
    return;
}

any consIntern(any x, any y)
{
    any r = cons(_CONTEXT_PTR, x, y);

    setCARType(r, INTERN);
    setCDRType(r, INTERN);

    return r;
}

/* Construct a cell */
any cons(Context *CONTEXT_PTR, any x, any y)
{
    cell *p;

    if (!(p = CONTEXT_PTR->Avail))
    {
        cell c1, c2;

        Push(c1,x);
        Push(c2,y);
        gc(CONTEXT_PTR, CELLS);
        drop(c1);
        p = CONTEXT_PTR->Avail;
    }
    CONTEXT_PTR->Avail = p->car;
    p->car = x;
    p->cdr = y;
    setCARType(p, PTR_CELL);
    setCDRType(p, PTR_CELL);
    return p;
}

/* Construct a symbol */
any consSym(Context *CONTEXT_PTR, any val, uword w)
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
    CONTEXT_PTR->Avail = p->car;
    p->cdr = val ? val : p;
    p->car = (any)w;
    setCARType(p, TXT);
    setCDRType(p, PTR_CELL);
    return p;
}

/* Construct a name cell */
any consName(Context *CONTEXT_PTR, uword w, any n)
{
   cell *p;

   if (!(p = CONTEXT_PTR->Avail))
   {
      gc(CONTEXT_PTR, CELLS);
      p = CONTEXT_PTR->Avail;
   }
   CONTEXT_PTR->Avail = p->car;
   p = symPtr(p);
   p->car = (any)w;
   p->cdr = n;
   setCARType(p, TXT);
   setCDRType(p, PTR_CELL);
   return p;
}

/* Allocate cell heap */
void heapAlloc(void)
{
   heap *h;
   cell *p;

   h = (heap*)((word)alloc(NULL, sizeof(heap) + sizeof(cell)) + (sizeof(cell)-1) & ~(sizeof(cell)-1));
   h->next = _CONTEXT_PTR->Heaps,  _CONTEXT_PTR->Heaps = h;
   p = h->cells + CELLS-1;
   do
   {
      Free(p);
   }
   while (--p >= h->cells);
}
