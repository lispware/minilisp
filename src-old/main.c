#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <setjmp.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>


/*******************************************************************************
DEFINITONS
*******************************************************************************/

/* Cell access */
#define car(x)          ((x)->car)
#define cdr(x)          ((x)->cdr)
#define caar(x)         (car(car(x)))
#define cadr(x)         (car(cdr(x)))
#define cdar(x)         (cdr(car(x)))
#define cddr(x)         (cdr(cdr(x)))
#define caaar(x)        (car(car(car(x))))
#define caadr(x)        (car(car(cdr(x))))
#define cadar(x)        (car(cdr(car(x))))
#define caddr(x)        (car(cdr(cdr(x))))
#define cdaar(x)        (cdr(car(car(x))))
#define cdadr(x)        (cdr(car(cdr(x))))
#define cddar(x)        (cdr(cdr(car(x))))
#define cdddr(x)        (cdr(cdr(cdr(x))))
#define cadddr(x)       (car(cdr(cdr(cdr(x)))))
#define cddddr(x)       (cdr(cdr(cdr(cdr(x)))))

#define data(c)         ((c).car)
#define Save(c)         ((c).cdr=CONTEXT_PTR->Env.stack, CONTEXT_PTR->Env.stack=&(c))
#define drop(c)         (CONTEXT_PTR->Env.stack=(c).cdr)
#define Push(c,x)       (data(c)=(x), Save(c))
#define Pop(c)          (drop(c), data(c))

#define Bind(s,f)       ((f).i=0, (f).cnt=1, (f).bnd[0].sym=(s), (f).bnd[0].val=val(s), (f).link=CONTEXT_PTR->Env.bind, CONTEXT_PTR->Env.bind=&(f))
#define Unbind(f)       (val((f).bnd[0].sym)=(f).bnd[0].val, CONTEXT_PTR->Env.bind=(f).link)

#define Nil ((CONTEXT_PTR->Mem))
#define T (&(CONTEXT_PTR->Mem[2]))
#define At (&(CONTEXT_PTR->Mem[3]))
#define At2 (&(CONTEXT_PTR->Mem[4]))
#define At3 (&(CONTEXT_PTR->Mem[5]))
#define doQuote_D (&(CONTEXT_PTR->Mem[6]))

#ifndef CELLS
#define CELLS (1024*1024/sizeof(cell))
#endif

#if INTPTR_MAX == INT32_MAX
    #define WORD_TYPE uint32_t
    #define SIGNED_WORD_TYPE int32_t
#elif INTPTR_MAX == INT64_MAX
    #define WORD_TYPE uint64_t
    #define SIGNED_WORD_TYPE int64_t
#else
    #error "Unsupported bit width"
#endif

#define LISP_WORD_SIZE ((int)sizeof(WORD_TYPE))
#define BITS (8*LISP_WORD_SIZE)

typedef SIGNED_WORD_TYPE word;
typedef WORD_TYPE uword;
typedef unsigned char byte;
typedef unsigned char *ptr;

typedef enum {NO,YES} bool;

typedef union
{
    unsigned char parts[4];
    uword _t;
}
PartType;

// PicoLisp primary data type
typedef struct _cell
{
   struct _cell *car;
   struct _cell *cdr;
   union
   {
       PartType type;
       struct _cell *ptr;
   } meta;
}
cell, *any;


struct _Context;

typedef any (*FunPtr)(struct _Context *, any);

typedef enum
{
    UNDEFINED,
    TXT,
    NUM,
    FUNC,
    PTR_CELL,
    INTERN,
    BIN,
    BIN_START,
} CellPartType;

typedef struct heap
{
   cell cells[CELLS];
   struct heap *next;
}
heap;

typedef struct bindFrame
{
   struct bindFrame *link;
   int i, cnt;
   struct {any sym; any val;} bnd[1];
}
bindFrame;

typedef struct inFrame
{
   struct inFrame *link;
   void (*get)(struct _Context *);
   FILE *fp;
   int next;
}
inFrame;

typedef struct outFrame
{
   struct outFrame *link;
   void (*put)(struct _Context*, int);
   FILE *fp;
}
outFrame;

typedef struct parseFrame
{
   int i;
   uword w;
   any sym, nm;
}
parseFrame;

typedef struct stkEnv
{
   cell *stack, *arg;
   bindFrame *bind;
   int next;
   any key, cls, *make, *yoke;
   inFrame *inFrames;
   outFrame *outFrames;
   parseFrame *parser;
   void (*get)(struct _Context*);
   void (*put)(struct _Context*, int);
   bool brk;
}
stkEnv;

typedef struct catchFrame
{
   struct catchFrame *link;
   any tag, fin;
   stkEnv env;
   jmp_buf rst;
}
catchFrame;

typedef struct
{
        any sym; any val;
}
bindFrameBind;

#define bindFrameSize (sizeof(bindFrame))
#define bindSize (sizeof(bindFrameBind))
static inline bindFrame *allocFrame(int l)
{
    int s1 = bindFrameSize;
    int s2 = (l - 1) * bindSize;
    return (bindFrame*)malloc(s1 + s2);
};

typedef struct _Context
{
    /* Globals */
    int Chr, Trace;
    char **AV, *AV0, *Home;
    heap *Heaps;
    cell *Avail;
    stkEnv Env;
    catchFrame *CatchPtr;
    FILE *InFile, *OutFile;
    any Intern[2], Transient[2];
    any ApplyArgs, ApplyBody;
    any Code;
    int HeapCount;
    any Mem;

}
Context;


/* Predicates */
#define isNil(x)        ((x)==Nil)
#define isTxt(x)        (((any)(x))->meta.type.parts[0] == TXT)
#define isNum(x)        (((any)(x))->meta.type.parts[0] == NUM)
#define isCell(x)        (((any)(x))->meta.type.parts[0] == PTR_CELL)
#define isFunc(x)        (((any)(x))->meta.type.parts[0] == FUNC)


/* Error checking */
#define NeedNum(ex,x)   if (!isNum(x)) numError(ex,x)
// #define NeedSym(ex,x)   if (!isSym(x)) symError(ex,x)
// #define NeedPair(ex,x)  if (!isCell(x)) pairError(ex,x)
// #define NeedAtom(ex,x)  if (isCell(x)) atomError(ex,x)
#define NeedLst(ex,x)   if (!isCell(x) && !isNil(x)) lstError(ex,x)
#define NeedVar(ex,x)   if (isNum(x)) varError(ex,x)

#define num(x)          ((word)(x))
#define tail(x)         (x)
#define val(x)          ((x)->cdr)
#define symPtr(x)       (x)
#define unBox(n)        (num(n->car))

void lstError(any,any) ;
/* Construct a cell */
#define evSubr(f,x)     (*(FunPtr)(num(f)))(CONTEXT_PTR, x)



void eofErr(void);
void printLongTXT(Context *CONTEXT_PTR, any nm);
uword length(Context *CONTEXT_PTR, any x);
any prog(Context *CONTEXT_PTR, any x);
any run(Context *CONTEXT_PTR, any x);
void pushInFiles(Context *CONTEXT_PTR, inFrame *f);
void popOutFiles(Context *CONTEXT_PTR);
void redefine(Context *CONTEXT_PTR, any ex, any s, any x);
void redefMsg(Context *CONTEXT_PTR, any x, any y);
void outString(Context *CONTEXT_PTR, char *s);
void print(Context *CONTEXT_PTR, any x);
void space(Context *CONTEXT_PTR);
void prin(Context *CONTEXT_PTR, any x);
void newline(Context *CONTEXT_PTR);
bool isSym(any x);
void rdOpen(Context *CONTEXT_PTR, any ex, any x, inFrame *f);
void popInFiles(Context *CONTEXT_PTR);
void pushOutFiles(Context *CONTEXT_PTR, outFrame *f);
void wrOpen(Context *CONTEXT_PTR, any ex, any x, outFrame *f);
any mkChar(Context *CONTEXT_PTR, int c);
void putByte1(int c, int *i, uword *p, any *q);
void putByte(Context *CONTEXT_PTR, int c, int *i, uword *p, any *q, cell *cp);
any popSym(Context *CONTEXT_PTR, int i, uword n, any q, cell *cp);
int firstByte(Context*CONTEXT_PTR, any s);
int getByte1(Context *CONTEXT_PTR, int *i, uword *p, any *q);
int getByte(Context *CONTEXT_PTR, int *i, uword *p, any *q);

any evList(Context *, any);
any EVAL(Context *CONTEXT_PTR, any x);
void err(any ex, any x, char *fmt, ...);
void undefined(any x, any ex);
void varError(any ex, any x);
void makeError(any ex);
void atomError(any ex, any x);
void lstError(any ex, any x);
any cons(Context *CONTEXT_PTR, any x, any y);
bool eol(Context *CONTEXT_PTR);
static void gc(Context *CONTEXT_PTR, word c);
uword getHeapSize(Context *CONTEXT_PTR);
any eqList(Context *CONTEXT_PTR, any v1, any v2);
void sym2str(Context *CONTEXT_PTR, any nm, char *buf);
void pack(Context *CONTEXT_PTR, any x, int *i, uword *p, any *q, cell *cp);
any name(any s);
void bufNum(char *b, word n);
void putByte0(int *i, uword *p, any *q);
any apply(Context *CONTEXT_PTR, any ex, any foo, bool cf, int n, cell *p);
any consSym(Context *CONTEXT_PTR, any val, uword w);
any load(Context *CONTEXT_PTR, any ex, int pr, any x);
any loadAll(Context *CONTEXT_PTR, any ex);

any doDe(Context *CONTEXT_PTR, any x);
any doAdd(Context *CONTEXT_PTR, any x);
any doSub(Context *CONTEXT_PTR, any x);
any doMul(Context *CONTEXT_PTR, any x);
any doLet(Context *CONTEXT_PTR, any x);
any doPrin(Context *CONTEXT_PTR, any x);
any doDo(Context *CONTEXT_PTR, any x);
any doSetq(Context *CONTEXT_PTR, any ex);
any doMake(Context *CONTEXT_PTR, any x);
any doLink(Context *CONTEXT_PTR, any x);
any doCons(Context *CONTEXT_PTR, any x);
any doCar(Context *CONTEXT_PTR, any ex);
any doCdr(Context *CONTEXT_PTR, any ex);
any doWhile(Context *CONTEXT_PTR, any x);
any doIn(Context *CONTEXT_PTR, any ex);
any doOut(Context *CONTEXT_PTR, any ex);
any doLine(Context *CONTEXT_PTR, any x);
any doChar(Context *CONTEXT_PTR, any ex);
any doNot(Context *CONTEXT_PTR, any x);
any doFor(Context *CONTEXT_PTR, any x);
any mkNum(Context *CONTEXT_PTR, word n);
any doRun(Context *CONTEXT_PTR, any x);
any doHS(Context *CONTEXT_PTR, any ignore);
any doEq(Context *CONTEXT_PTR, any x);
any doIf(Context *CONTEXT_PTR, any x);
any doCall(Context *CONTEXT_PTR, any ex);
any doPack(Context *CONTEXT_PTR, any x);
any doEval(Context *CONTEXT_PTR, any x);
any doMapcar(Context *CONTEXT_PTR, any ex);
any doChop(Context *CONTEXT_PTR, any x);
any doLoop(Context *CONTEXT_PTR, any ex);
any doLoad(Context *CONTEXT_PTR, any ex);
any doArgs(Context *CONTEXT_PTR, any ex);
any doNext(Context *CONTEXT_PTR, any ex);


/******************************************************************************/

/*******************************************************************************
system functions
*******************************************************************************/

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

void lstError(any ex, any x)
{
    err(ex, x, "List expected");
}

void numError(any ex, any x)
{
    err(ex, x, "Number expected");
}

void varError(any ex, any x)
{
    err(ex, x, "Variable expected");
}

void makeError(any ex)
{
    err(ex, NULL, "Not making");
}

void atomError(any ex, any x)
{
    err(ex, x, "Atom expected");
}

/*** Error handling ***/
void err(any ex, any x, char *fmt, ...)
{
    printf("ERROR\n");
    bye(0);
    if (ex == x) bye(1);
    if (fmt == NULL) bye(1);
}



/******************************************************************************/



/*******************************************************************************
Cell functions
*******************************************************************************/

CellPartType getCARType(any cell)
{
    return cell->meta.type.parts[0];
}

CellPartType getCDRType(any cell)
{
    return cell->meta.type.parts[1];
}

void setCARType(any cell, CellPartType type)
{
    if (!cell) return;
    cell->meta.type.parts[0] = type;
}

void setCDRType(any cell, CellPartType type)
{
    if (!cell) return;
    cell->meta.type.parts[1] = type;
}

/******************************************************************************/


/*******************************************************************************
c functions
*******************************************************************************/

any doNot(Context *CONTEXT_PTR, any x)
{
   any a;

   if (isNil(a = EVAL(CONTEXT_PTR, cadr(x))))
      return T;
   val(At) = a;
   return Nil;
}

// (quote . any) -> any
any doQuote(Context *CONTEXT_PTR, any x)
{
    return cdr(x);
}

// (bye 'num|NIL)
any doBye(Context *CONTEXT_PTR, any ex)
{
   printf("\n");
   bye(0);
   return ex;
}

// (de sym . any) -> sym
any doDe(Context *CONTEXT_PTR, any ex)
{
    any s = cadr(ex);
    val(s) = cddr(ex);
    return s;
}

any mkNum(Context *CONTEXT_PTR, word n)
{
    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)n;
    r->meta.type.parts[0] = NUM;
    return r;
}

// (+ 'num ..) -> num
any doAdd(Context *CONTEXT_PTR, any ex)
{
    any x, y;
    uword n=0;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = unBox(y);
    while (Nil != (x = cdr(x)))
    {
        if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
            return Nil;
        NeedNum(ex,y);
        n += unBox(y);
    }

    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)n;
    r->meta.type.parts[0] = NUM;
    return r;
}

any doSub(Context *CONTEXT_PTR, any ex)
{
    any x, y;
    uword n=0;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = unBox(y);
    while (Nil != (x = cdr(x)))
    {
        if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
            return Nil;
        NeedNum(ex,y);
        n -= unBox(y);
    }

    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)n;
    r->meta.type.parts[0] = NUM;
    return r;
}

any doMul(Context *CONTEXT_PTR, any ex)
{
    any x, y;
    uword n=0;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;
    NeedNum(ex,y);
    n = unBox(y);
    while (Nil != (x = cdr(x)))
    {
        if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
            return Nil;
        NeedNum(ex,y);
        n *= unBox(y);
    }

    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)n;
    r->meta.type.parts[0] = NUM;
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

// (char) -> sym
// (char 'num) -> sym
// (char 'sym) -> num
any doChar(Context *CONTEXT_PTR, any ex)
{
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
   return Nil;
}

// (line 'flg) -> lst|sym
any doLine(Context *CONTEXT_PTR, any x)
{
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
         val(f->bnd[1].sym) = Nil;
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
      val(f->bnd[1].sym) = Nil;
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
    return ignore;
}

any doRun(Context *CONTEXT_PTR, any x)
{
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

any eqList(Context *CONTEXT_PTR, any v1, any v2)
{
    while(v1 != Nil)
    {
        any x1 = car(v1);
        if (getCARType(x1) == PTR_CELL)
        {
            any r = eqList(CONTEXT_PTR, x1, car(v2));
            if (r != T) return Nil;
        }
        else
        {
            if (car(x1) != car(car(v2))) return Nil;
        }

        v1 = cdr(v1);
        v2 = cdr(v2);
    }

    if (v2 != Nil) return Nil;

    if (v2 != Nil) return Nil;

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
        else if (t == PTR_CELL)
        {
            drop(c1);
            return eqList(CONTEXT_PTR, v, v2);
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

// (mapcar 'fun 'lst ..) -> lst
any doMapcar(Context *CONTEXT_PTR, any ex)
{
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

    if(x==Nil) return Nil;

    c = getByte1(CONTEXT_PTR, &i, &w, &x);
    Push(c1, cons(CONTEXT_PTR, mkChar(CONTEXT_PTR, c), Nil));
    y = data(c1);
    while (c)
    {
        c = getByte(CONTEXT_PTR,&i, &w, &x);
        if (c)
        {
            y = cdr(y) = cons(CONTEXT_PTR, mkChar(CONTEXT_PTR, c), Nil);
        }
    }

    return Pop(c1);

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

/******************************************************************************/


/*******************************************************************************
Setup built in functions
*******************************************************************************/

int MEMS;
any Mem;

bool isSym(any x)
{
   if (x) return 0;
   // TODO - this must be checked out
   return 0;
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


void setupBuiltinFunctions()
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
    // xxx = addKeyVal3(xxx, "dump", doDump);
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
    // xxx = addKeyVal3(xxx, "fork", doFork);
    // xxx = addKeyVal3(xxx, "sleep", doSleep);
    // xxx = addKeyVal3(xxx, "io", doIO);
    xxx = addKeyVal3(xxx, "load", doLoad);
    xxx = addKeyVal3(xxx, "eval", doEval);
    xxx = addKeyVal3(xxx, "mapcar", doMapcar);
    // xxx = addKeyVal3(xxx, "sampleOpen", doSampleOpen);
    // xxx = addKeyVal3(xxx, "sampleRead", doSampleRead);
    // xxx = addKeyVal3(xxx, "bind", doBind);
    // xxx = addKeyVal3(xxx, "listen", doListen);
    // xxx = addKeyVal3(xxx, "skt", doSocket);
    // xxx = addKeyVal3(xxx, "connect", doConnect);
    // xxx = addKeyVal3(xxx, "http", doHTTP);
    // xxx = addKeyVal3(xxx, "sktClose", doSocketClose);
    xxx = addKeyVal3(xxx, "loop", doLoop);
    xxx = addKeyVal3(xxx, "chop", doChop);
    // xxx = addKeyVal3(xxx, "gc", doGC);

    // xxx = addKeyVal4(xxx, "Z", 10);

    // xxx = addKeyVal4(xxx, "ABCDEFGHIJ", 10);
    // xxx = addKeyVal3(xxx, "ABCDEFGHIJK", doLongFunc);
    // xxx = addKeyVal3(xxx, "ABCDEFGHIJABCDEFGHIJK", doVeryLongFunc);

    xxx = addKeyVal3(xxx, "args", doArgs);
    xxx = addKeyVal3(xxx, "next", doNext);
    
    WORD_TYPE end = (WORD_TYPE)xxx;
    WORD_TYPE start = (WORD_TYPE)Mem;
    MEMS = (end - start)/sizeof(cell);
}


/******************************************************************************/




/*******************************************************************************
gc functions
*******************************************************************************/

/* Allocate cell heap */
void heapAlloc(Context *CONTEXT_PTR)
{
   heap *h;
   cell *p;

   CONTEXT_PTR->HeapCount++;
   //h = (heap*)((word)alloc(NULL, sizeof(heap) + sizeof(cell)) + (sizeof(cell)-1) & ~(sizeof(cell)-1));
   h = (heap*)((word)calloc(1, sizeof(heap) + sizeof(cell)));
   h->next = CONTEXT_PTR->Heaps,  CONTEXT_PTR->Heaps = h;
   p = h->cells + CELLS-1;
   do
   {
      //Free(p);
      p->car=CONTEXT_PTR->Avail;
      CONTEXT_PTR->Avail = p;
   }
   while (--p >= h->cells);
}

void setMark(any cell, int m)
{
    cell->meta.type.parts[3] = m;
}

int getMark(any cell)
{
    return cell->meta.type.parts[3];
}


static void mark(Context *CONTEXT_PTR, any x)
{
    if (!x) return;

    if (getMark(x)) return;

    setMark(x, 1);

    if (x == Nil) return;

    if (getCARType(x) == BIN_START)
    {
        if (getCDRType(x) == PTR_CELL) mark(CONTEXT_PTR, cdr(x));
        x = x->car;
        while(x && x != Nil)
        {
            mark(CONTEXT_PTR, x);
            x=x->cdr;
        }
        return;
    }


    if (getCARType(x) == PTR_CELL || getCARType(x) == INTERN) mark(CONTEXT_PTR, car(x));

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
            mark(CONTEXT_PTR, x);
        }
        if (getCARType(x) == PTR_CELL || getCARType(x) == INTERN) mark(CONTEXT_PTR, car(x));
    }
}

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
   for (p = (any)CONTEXT_PTR->CatchPtr; p; p = (any)((catchFrame*)p)->link)
   {
      if (((catchFrame*)p)->tag)
         mark(CONTEXT_PTR, ((catchFrame*)p)->tag);
      mark(CONTEXT_PTR, ((catchFrame*)p)->fin);
   }
}

/* Garbage collector */
void gc(Context *CONTEXT_PTR, word c)
{
    any p;
    heap *h;

    //doDump(CONTEXT_PTR, Nil);
    markAll(CONTEXT_PTR);
    //doDump(CONTEXT_PTR, Nil);

    /* Sweep */
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
                    memset(p, 0, sizeof(cell));
                    p->car = CONTEXT_PTR->Avail;
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

    //doDump(CONTEXT_PTR, Nil);
    return;
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


/******************************************************************************/

/*******************************************************************************
sym functions
*******************************************************************************/

any doHide(Context* CONTEXT_PTR, any ex)
{
   // TODO - is this needed?
   // printf("%p\n", ex);

   return Nil;
}

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


/* Get symbol name */
any name(any s)
{
   return s;
}

any consIntern(Context *CONTEXT_PTR, any x, any y)
{
    any r = cons(CONTEXT_PTR, x, y);

    setCARType(r, PTR_CELL);
    setCDRType(r, PTR_CELL);

    return r;
}



int firstByte(Context*CONTEXT_PTR, any s)
{
    if (getCARType(s) == TXT)
    {
        return ((uword)(s->car)) & 127;
    }
    else if (getCARType(s) == BIN_START)
    {
        return ((uword)(s->car->car)) & 127;
    }
    else
    {
        giveup("Cant get first byte");
        return -1;
    }
}

int getByte1(Context *CONTEXT_PTR, int *i, uword *p, any *q)
{
    int c;

    if (getCARType(*q) == TXT)
    {
        (*q)=(*q)->car;
        *i = BITS, *p = (uword)(*q) , *q = NULL;
    }
    else if (getCARType(*q) == BIN_START)
    {
        (*q)=(*q)->car;
        *i = BITS, *p = (uword)((*q)->car) , *q = ((*q)->cdr);
    }
    else
    {
        giveup("Cant getByte");
    }

    c = *p & 127, *p >>= 8, *i -= 8;

    return c;
}

int getByte(Context *CONTEXT_PTR, int *i, uword *p, any *q)
{
    int c;

    if (*i == 0)
    {
        if (!*q || *q == Nil)
        {
            return 0;
        }
        else
        {
            *i = BITS,  *p = (uword)((*q)->car),  *q = (*q)->cdr;
        }
    }
    c = *p & 127,  *p >>= 8;
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
    c &= 127;

    return c;
}

any mkChar(Context *CONTEXT_PTR, int c)
{
   return consSym(CONTEXT_PTR, NULL, c & 127);
}

void putByte0(int *i, uword *p, any *q)
{
    *p = 0;
    *i = 0;
    *q = NULL;
}

void putByte1(int c, int *i, uword *p, any *q)
{
    *p = c & 127;
    *i = 8;
    *q = NULL;
}

void putByte(Context *CONTEXT_PTR, int c, int *i, uword *p, any *q, cell *cp)
{
    c = c & 127;
    int d = 8;

    if (*i != BITS)
        *p |= (uword)c << *i;

    if (*i + d  > BITS)
    {
        if (*q)
        {
            any x = consName(CONTEXT_PTR, *p, Nil);
            setCARType(x, BIN);
            (*q)->cdr = x;
            *q = x;
        }
        else
        {
            any x = consSym(CONTEXT_PTR, NULL, 0);
            setCARType(x, BIN_START);
            Push(*cp, x);
            any y = consName(CONTEXT_PTR, *p, Nil);
            setCARType(y, BIN);
            (*cp).car->car = *q = y;

        }
        *p = c >> BITS - *i;
        *i -= BITS;
    }

    *i += d;
}

any popSym(Context *CONTEXT_PTR, int i, uword n, any q, cell *cp)
{
    if (q)
    {
        //val(q) = i <= (BITS-2)? box(n) : consName(CONTEXT_PTR, n, Zero);
        q->cdr = consName(CONTEXT_PTR, n, Nil);
        return Pop(*cp);
    }
    return consSym(CONTEXT_PTR, NULL,n);
}

int symBytes(Context *CONTEXT_PTR, any x)
{
    int cnt = 0;
    uword w;

    if (isNil(x))
        return 0;

    CellPartType t = getCARType(x);

    if (t == TXT)
    {
        w = (uword)(x->car);
        while (w)
        {
            ++cnt;
            w >>= 8;
        }
    }
    else if (t == BIN_START)
    {

        x = x->car;
        while (x != Nil)
        {
			w = (uword)(x->car);
            while (w)
            {
                ++cnt;
                w >>= 8;
            }
            x = x->cdr;
        }
    }

    return cnt;
}

any isIntern(Context *CONTEXT_PTR, any nm, any tree[2])
{
    any x, y, z;
    word n;

    if (isTxt(nm))
    {
        for (x = tree[0];  x != Nil;)
        {
            if ((n = (word)(car(nm)) - (word)name(caar(x))) == 0)
            {
                return car(x);
            }
            x = n<0? cadr(x) : cddr(x);
        }
    }
    else
    {
        for (x = tree[1];  x != Nil;)
        {
            y = nm->car;
            z = x->car->car;
            while ((n = (word)(car(y)) - (word)car(z)) == 0)
            {
                if (getCARType(y) != BIN) return car(x);
                y=y->cdr;
                z=z->cdr;
            }

            x = n<0? cadr(x) : cddr(x);
        }
    }



    return NULL;
}

any internBin(Context *CONTEXT_PTR, any sym, any tree[2])
{
    any nm, x, y, z;
    word n;

    x = tree[1];

    if (x == Nil)
    {
        tree[1] = consIntern(CONTEXT_PTR, sym, Nil);
        return tree[1];
    }

    for (;;)
    {

        y = sym->car;
        z = x->car->car;
        while ((n = (word)(car(y)) - (word)car(z)) == 0)
        {
            if (getCARType(y) != BIN) return sym;
            y=y->cdr;
            z=z->cdr;
        }

        if (Nil == cdr(x))
        {
            cdr(x) = n < 0 ? consIntern(CONTEXT_PTR, consIntern(CONTEXT_PTR, sym, Nil), Nil) : consIntern(CONTEXT_PTR, Nil, consIntern(CONTEXT_PTR, sym, Nil));
            return sym;
        }
        if (n < 0)
        {
            if (Nil != cadr(x))
            {
                x = cadr(x);
            }
            else
            {
                cadr(x) = consIntern(CONTEXT_PTR, sym, Nil);
                return sym;
            }
        }
        else
        {
            if (Nil != cddr(x))
            {
                x = cddr(x);
            }
            else
            {
                cddr(x) = consIntern(CONTEXT_PTR, sym, Nil);
                return sym;
            }
        }
    }
}

any intern(Context *CONTEXT_PTR, any sym, any tree[2])
{
   any nm, x;
   word n;


   if (getCARType(sym) == BIN_START) return internBin(CONTEXT_PTR, sym, tree);

   nm = sym;

   x = tree[0];
   if (Nil == x)
   {
      tree[0] = consIntern(CONTEXT_PTR, sym, Nil);
      return tree[0];
   }
   for (;;)
   {
      if ((n = (word)(car(nm)) - (word)name(caar(x))) == 0)
      {
         return car(x);
      }

      if (Nil == cdr(x))
      {
         cdr(x) = n < 0 ? consIntern(CONTEXT_PTR, consIntern(CONTEXT_PTR, sym, Nil), Nil) : consIntern(CONTEXT_PTR, Nil, consIntern(CONTEXT_PTR, sym, Nil));
         return sym;
      }
      if (n < 0)
      {
         if (Nil != cadr(x))
         {
            x = cadr(x);
         }
         else
         {
            cadr(x) = consIntern(CONTEXT_PTR, sym, Nil);
            return sym;
         }
      }
      else
      {
         if (Nil != cddr(x))
         {
            x = cddr(x);
         }
         else
         {
            cddr(x) = consIntern(CONTEXT_PTR, sym, Nil);
            return sym;
         }
      }
   }
}

/* Make name */
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

/* Make string */
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

/* Make number from symbol */
any symToNum(Context *CONTEXT_PTR, any sym, int scl, int sep, int ign)
{
    unsigned c;
    int i;
    uword w;
    bool sign, frac;
    word n;
    any s = sym;


    if (!(c = getByte1(CONTEXT_PTR, &i, &w, &s)))
        return NULL;

    while (c <= ' ')  /* Skip white space */
    {
        if (!(c = getByte(CONTEXT_PTR, &i, &w, &s)))
            return NULL;
    }

    sign = NO;
    if (c == '+'  ||  c == '-' && (sign = YES))
    {
        if (!(c = getByte(CONTEXT_PTR, &i, &w, &s)))
            return NULL;
    }

    if ((c -= '0') > 9)
        return NULL;

    frac = NO;
    n = c;
    while ((c = getByte(CONTEXT_PTR, &i, &w, &s))  &&  (!frac || scl))
    {
        if ((int)c == sep)
        {
            if (frac)
                return NULL;
            frac = YES;
        }
        else if ((int)c != ign)
        {
            if ((c -= '0') > 9)
                return NULL;
            n = n * 10 + c;
            if (frac)
                --scl;
        }
    }
    if (c)
    {
        if ((c -= '0') > 9)
            return NULL;
        if (c >= 5)
            n += 1;
        while (c = getByte(CONTEXT_PTR, &i, &w, &s))
        {
            if ((c -= '0') > 9)
                return NULL;
        }
    }
    if (frac)
        while (--scl >= 0)
            n *= 10;


    any r = cons(CONTEXT_PTR, Nil, Nil);
    r->car = (any)n;
    r->meta.type.parts[0] = NUM;

    return r;
}

/******************************************************************************/


/*******************************************************************************
eval functions
*******************************************************************************/
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

void undefined(any x, any ex)
{
    err(ex, x, "Undefined");
}

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

// (args) -> flg
any doArgs(Context *CONTEXT_PTR, any ex) {
   return CONTEXT_PTR->Env.next > 0? T : Nil;
}

// (next) -> any
any doNext(Context *CONTEXT_PTR, any ex) {
   if (CONTEXT_PTR->Env.next > 0)
      return data(CONTEXT_PTR->Env.arg[--CONTEXT_PTR->Env.next]);
   if (CONTEXT_PTR->Env.next == 0)
      CONTEXT_PTR->Env.next = -1;
   return Nil;
}


/******************************************************************************/


/*******************************************************************************
read functions
*******************************************************************************/

bool eol(Context *CONTEXT_PTR)
{
   if (CONTEXT_PTR->Chr < 0)
      return YES;
   if (CONTEXT_PTR->Chr == '\n')
   {
      CONTEXT_PTR->Chr = 0;
      return YES;
   }
   if (CONTEXT_PTR->Chr == '\r')
   {
      CONTEXT_PTR->Env.get(CONTEXT_PTR);
      if (CONTEXT_PTR->Chr == '\n')
         CONTEXT_PTR->Chr = 0;
      return YES;
   }
   return NO;
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

static any read0(Context *, bool);

static char Delim[] = " \t\n\r\"'(),[]`~{}";

/* Buffer size */
int bufSize(Context *CONTEXT_PTR, any x)
{
    return symBytes(CONTEXT_PTR, x) + 1;
}

int pathSize(Context *CONTEXT_PTR, any x)
{
    int c = firstByte(CONTEXT_PTR, x);

    if (c != '@'  &&  (c != '+'))
    {
        return bufSize(CONTEXT_PTR, x);
    }
    if (!CONTEXT_PTR->Home)
    {
        return symBytes(CONTEXT_PTR, x);
    }
    return strlen(CONTEXT_PTR->Home) + symBytes(CONTEXT_PTR, x);
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

static int skip(Context *CONTEXT_PTR)
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

/* Test for escaped characters */
static bool testEsc(Context *CONTEXT_PTR)
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

/* Read a list */
static any rdList(Context *CONTEXT_PTR)
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
        if (isCell(x = data(c1) = EVAL(CONTEXT_PTR, data(c1))))
        {
            while (isCell(cdr(x)))
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
            x = cdr(x) = cons(CONTEXT_PTR, read0(CONTEXT_PTR, NO),Nil);
        }
        else
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
            cdr(x) = read0(CONTEXT_PTR, NO);
            cdr(x) = EVAL(CONTEXT_PTR, cdr(x));
            while (isCell(cdr(x)))
            {
                x = cdr(x);
            }
        }
    }
    return Pop(c1);
}

/* Read one expression */
static any read0(Context *CONTEXT_PTR, bool top)
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
        return cons(CONTEXT_PTR, doQuote_D, read0(CONTEXT_PTR, top));
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

    y = popSym(CONTEXT_PTR, i, w, p, &c1);
    //printf("%p --> CAR = %p CDR = %p \n", y, y->car, y->cdr);
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

/******************************************************************************/


/*******************************************************************************
I/O io functions
*******************************************************************************/

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

void openErr(any ex, char *s)
{
    err(ex, NULL, "%s open: %s", s, strerror(errno));
}

void eofErr(void)
{
    err(NULL, NULL, "EOF Overrun");
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
         if (!(f->fp = fopen(nm+1, "a")))
            openErr(ex, nm);
      }
      else if (!(f->fp = fopen(nm, "w")))
         openErr(ex, nm);

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


void putStdout(Context *CONTEXT_PTR, int c)
{
    putc(c, CONTEXT_PTR->OutFile);
}

void getStdin(Context *CONTEXT_PTR)
{
    CONTEXT_PTR->Chr = getc(CONTEXT_PTR->InFile);
}

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

void popOutFiles(Context *CONTEXT_PTR)
{
    if (CONTEXT_PTR->OutFile != stdout && CONTEXT_PTR->OutFile != stderr)
    {
        fclose(CONTEXT_PTR->OutFile);
    }
    CONTEXT_PTR->Env.put = CONTEXT_PTR->Env.outFrames->put;
    CONTEXT_PTR->OutFile = (CONTEXT_PTR->Env.outFrames = CONTEXT_PTR->Env.outFrames->link)? CONTEXT_PTR->Env.outFrames->fp : stdout;
}


/******************************************************************************/

/*******************************************************************************
main functions
*******************************************************************************/
 

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


Context LISP_CONTEXT;




int main(int argc, char *av[])
{
    setupBuiltinFunctions();

    Context *CONTEXT_PTR = &LISP_CONTEXT;
    CONTEXT_PTR->Mem = Mem;
    copy_mem(Mem, CONTEXT_PTR);
    initialize_context(CONTEXT_PTR);
    av++;
    CONTEXT_PTR->AV = av;

    CONTEXT_PTR->InFile = stdin, CONTEXT_PTR->Env.get = getStdin;
    CONTEXT_PTR->OutFile = stdout, CONTEXT_PTR->Env.put = putStdout;
    CONTEXT_PTR->ApplyArgs = cons(CONTEXT_PTR, cons(CONTEXT_PTR, consSym(CONTEXT_PTR, Nil, 0), Nil), Nil);
    CONTEXT_PTR->ApplyBody = cons(CONTEXT_PTR, Nil, Nil);



    loadAll(CONTEXT_PTR, NULL);
    while (!feof(stdin))
        load(CONTEXT_PTR, NULL, ':', Nil);
    bye(0);

    return 0;
}
