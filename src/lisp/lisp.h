#ifndef __LISP_H__
#define __LISP_H__

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
#define CELLS (1024*1024)
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

#define getCARType(C) ((C)->meta.type.parts[0])
#define getCDRType(C) ((C)->meta.type.parts[1])
#define setCARType(C, V) ((C)->meta.type.parts[0] = V)
#define setCDRType(C, V) ((C)->meta.type.parts[1] = V)


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
    EXT,
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
}

typedef struct _Context
{
    /* Globals */
    int Chr, Trace;
    char **AV, *AV0;
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

typedef enum
{
    EXT_SOCKET,
} EXT_TYPE;

typedef struct _external
{
    EXT_TYPE type;
    void *pointer;
    void (*release)(struct _external*);
    char *(*print)(Context *, struct _external*);
} external;


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
void numError(any,any) ;
void openErr(any ex, char *s);
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
any copyNum(Context *CONTEXT_PTR, any n);
void putByte1(int c, int *i, uword *p, any *q);
void putByte(Context *CONTEXT_PTR, int c, int *i, uword *p, any *q, cell *cp);
any popSym(Context *CONTEXT_PTR, int i, uword n, any q, cell *cp);
int firstByte(Context*CONTEXT_PTR, any s);
int getByte1(Context *CONTEXT_PTR, int *i, uword *p, any *q);
int getByte(Context *CONTEXT_PTR, int *i, uword *p, any *q);
void pathString(Context *CONTEXT_PTR, any x, char *p);
int bufSize(Context *CONTEXT_PTR, any x);
void comment(Context *CONTEXT_PTR);
int pathSize(Context *CONTEXT_PTR, any x);
bool testEsc(Context *CONTEXT_PTR);
int skip(Context *CONTEXT_PTR);
any read0(Context *CONTEXT_PTR, bool top);
any read1(Context *CONTEXT_PTR, int end);
any rdList(Context *CONTEXT_PTR);

any evList(Context *, any);
any evList2(Context *CONTEXT_PTR, any foo, any ex);
any EVAL(Context *CONTEXT_PTR, any x);
void err(any ex, any x, char *fmt, ...);
void undefined(any x, any ex);
void varError(any ex, any x);
void makeError(any ex);
void atomError(any ex, any x);
void lstError(any ex, any x);
void giveup(char *msg);
void bye(int n);
any cons(Context *CONTEXT_PTR, any x, any y);
bool eol(Context *CONTEXT_PTR);
void gc(Context *CONTEXT_PTR, word c);
uword getHeapSize(Context *CONTEXT_PTR);
int eqList(Context *CONTEXT_PTR, any v1, any v2);
void sym2str(Context *CONTEXT_PTR, any nm, char *buf);
void pack(Context *CONTEXT_PTR, any x, int *i, uword *p, any *q, cell *cp);
any name(any s);
void bufNum(char *b, word n);
void putByte0(int *i, uword *p, any *q);
any apply(Context *CONTEXT_PTR, any ex, any foo, bool cf, int n, cell *p);
any consSym(Context *CONTEXT_PTR, any val, uword w);
any load(Context *CONTEXT_PTR, any ex, int pr, any x);
any loadAll(Context *CONTEXT_PTR, any ex);
any addString(any *, any m, char *s);
any addLongString(any *, any m, char *s);
any addShortString(any m, char *s);
any addKeyVal1(any m, char *s);
any addKeyVal2(any m, char *s);
any addKeyVal3(any m, char *s, void *v);
any addKeyVal4(any m, char *s, WORD_TYPE num);
void setupBuiltinFunctions(any *);
void setMark(any cell, int m);
int getMark(any cell);
void markAll(Context *CONTEXT_PTR);
void mark(Context *CONTEXT_PTR, any x);
void heapAlloc(Context *CONTEXT_PTR);
void copy_mem(any M, Context *To);
any isIntern(Context *CONTEXT_PTR, any nm, any tree[2]);
any intern(Context *CONTEXT_PTR, any sym, any tree[2]);
any internBin(Context *CONTEXT_PTR, any sym, any tree[2]);
void initialize_context(Context *CONTEXT_PTR);
any consIntern(Context *CONTEXT_PTR, any x, any y);
any consName(Context *CONTEXT_PTR, uword w, any n);
int symBytes(Context *CONTEXT_PTR, any x);
any symToNum(Context *CONTEXT_PTR, any sym, int scl, int sep, int ign);
any mkSym(Context *CONTEXT_PTR, byte *s);
any mkStr(Context *CONTEXT_PTR, char *s);
any evExpr(Context *CONTEXT_PTR, any expr, any x);
void outNum(Context *CONTEXT_PTR, any n);
void getStdin(Context *CONTEXT_PTR);
void putStdout(Context *CONTEXT_PTR, int c);
int equal(Context *CONTEXT_PTR, any v, any v2);
any indx(Context *CONTEXT_PTR, any x, any y);
any doPopq(Context *CONTEXT_PTR, any x);

any doDe(Context *CONTEXT_PTR, any x);
any doNumLt(Context *CONTEXT_PTR, any x);
any doNumGt(Context *CONTEXT_PTR, any x);
any doInc(Context *CONTEXT_PTR, any x);
any doDec(Context *CONTEXT_PTR, any x);
any doAdd(Context *CONTEXT_PTR, any x);
any doSub(Context *CONTEXT_PTR, any x);
any doMul(Context *CONTEXT_PTR, any x);
any doDiv(Context *CONTEXT_PTR, any x);
any doMod(Context *CONTEXT_PTR, any x);
any doBinRShift(Context *CONTEXT_PTR, any ex);
any doBinNot(Context *CONTEXT_PTR, any x);
any doBinAnd(Context *CONTEXT_PTR, any x);
any doBinOr(Context *CONTEXT_PTR, any x);
any doBinXor(Context *CONTEXT_PTR, any x);
any doPow(Context *CONTEXT_PTR, any x);
any doRandom(Context *CONTEXT_PTR, any x);
any doLet(Context *CONTEXT_PTR, any x);
any doPrin(Context *CONTEXT_PTR, any x);
any doCall(Context *CONTEXT_PTR, any ex);
any doDo(Context *CONTEXT_PTR, any x);
any doSetq(Context *CONTEXT_PTR, any ex);
any doMake(Context *CONTEXT_PTR, any x);
any doIndex(Context *CONTEXT_PTR, any x);
any doLink(Context *CONTEXT_PTR, any x);
any doCons(Context *CONTEXT_PTR, any x);
any doCar(Context *CONTEXT_PTR, any ex);
any doCdr(Context *CONTEXT_PTR, any ex);
any doWhile(Context *CONTEXT_PTR, any x);
any doIn(Context *CONTEXT_PTR, any ex);
any doOut(Context *CONTEXT_PTR, any ex);
any doLine(Context *CONTEXT_PTR, any x);
any doChar(Context *CONTEXT_PTR, any ex);
any doSwitchBase(Context *CONTEXT_PTR, any ex);
any doRd(Context *CONTEXT_PTR, any ex);
any doWr(Context *CONTEXT_PTR, any ex);
any doNot(Context *CONTEXT_PTR, any x);
any doQuote(Context *CONTEXT_PTR, any x);
any doFor(Context *CONTEXT_PTR, any x);
any mkNum(Context *CONTEXT_PTR, word n);
any doRun(Context *CONTEXT_PTR, any x);
any doHS(Context *CONTEXT_PTR, any ignore);
any doAnd(Context *CONTEXT_PTR, any x);
any doOr(Context *CONTEXT_PTR, any x);
any doEq(Context *CONTEXT_PTR, any x);
any doCmp(Context *CONTEXT_PTR, any x);
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
any doBye(Context *CONTEXT_PTR, any ex);
any doHide(Context* CONTEXT_PTR, any ex);
any doThread(Context* CONTEXT_PTR, any ex);
any doSleep(Context* CONTEXT_PTR, any ex);
any doBind(Context *CONTEXT_PTR, any ex);
any doListen(Context *CONTEXT_PTR, any ex);
any doSocket(Context *CONTEXT_PTR, any ex);
any doConnect(Context *CONTEXT_PTR, any ex);
any doTid(Context *CONTEXT_PTR, any ex);

extern int MEMS;
extern any Mem;


/******************************************************************************/

#endif