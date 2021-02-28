#ifndef __LISP_H__
#define __LISP_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <stdint.h>

#ifndef CELLS
#define CELLS (1024*1024/sizeof(cell))
#endif

#if INTPTR_MAX == INT32_MAX
    #define WORD_TYPE uint32_t
    #define SIGNED_WORD_TYPE int32_t
    #define WORD_FORMAT_STRING "0x%x"
    #define WORD_FORMAT_STRING_D "%d"
#elif INTPTR_MAX == INT64_MAX
    #define WORD_TYPE uint64_t
    #define SIGNED_WORD_TYPE int64_t
    #define WORD_FORMAT_STRING "0x%llx"
    #define WORD_FORMAT_STRING_D "%lld"
#else
    #error "Unsupported bit width"
#endif

#define WORD ((int)sizeof(WORD_TYPE))
#define BITS (8*WORD)

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
   PartType type;
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
   void (*put)(int);
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
   void (*put)(int);
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


/* Number access */
#define num(x)          ((word)(x))
#define txt(n)          ((any)(num(n)<<1|1))
#define box(n)          ((any)(num(n)<<2|2))
#define unBox(n)        (num(n->car))

/* Symbol access */
#define symPtr(x)       (x)
#define val(x)          ((x)->cdr)

#define tail(x)         (x)

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
#define Save(c)         ((c).cdr=CONTEXT.Env.stack, CONTEXT.Env.stack=&(c))
#define drop(c)         (CONTEXT.Env.stack=(c).cdr)
#define Push(c,x)       (data(c)=(x), Save(c))
#define Pop(c)          (drop(c), data(c))

#define Bind(s,f)       ((f).i=0, (f).cnt=1, (f).bnd[0].sym=(s), (f).bnd[0].val=val(s), (f).link=CONTEXT.Env.bind, CONTEXT.Env.bind=&(f))
#define Unbind(f)       (val((f).bnd[0].sym)=(f).bnd[0].val, CONTEXT.Env.bind=(f).link)

/* Predicates */
#define isNil(x)        ((x)==Nil)
#define isTxt(x)        (((any)(x))->type.parts[0] == TXT)
#define isNum(x)        (((any)(x))->type.parts[0] == NUM)
#define isCell(x)        (((any)(x))->type.parts[0] == PTR_CELL)
#define isFunc(x)        (((any)(x))->type.parts[1] == FUNC)


/* Error checking */
#define NeedNum(ex,x)   if (!isNum(x)) numError(ex,x)
#define NeedSym(ex,x)   if (!isSym(x)) symError(ex,x)
#define NeedPair(ex,x)  if (!isCell(x)) pairError(ex,x)
#define NeedAtom(ex,x)  if (isCell(x)) atomError(ex,x)
#define NeedLst(ex,x)   if (!isCell(x) && !isNil(x)) lstError(ex,x)
#define NeedVar(ex,x)   if (isNum(x)) varError(ex,x)

void lstError(any,any) ;
/* Construct a cell */
#define evSubr(f,x)     (*(FunPtr)(num(f)))(_CONTEXT_PTR, x)

/* Globals */
// extern int Chr, Trace;
// extern char **AV, *AV0, *Home;
// extern heap *Heaps;
// extern cell *Avail;
// extern stkEnv Env;
// extern catchFrame *CatchPtr;
// extern FILE *InFile, *OutFile;
// extern any TheKey, TheCls, Thrown;
// extern any Intern[2], Transient[2];
// extern any ApplyArgs, ApplyBody;

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
}
Context;


void makeError(any ex);
void varError(any,any) ;
void numError(any,any) ;
int getByte1(int *i, uword *p, any *q);
int getByte(int *i, uword *p, any *q);

any doFor(Context*, any x);
any doSetq(Context*, any x);


bool isSym(any x);


any doAdd(Context*, any ex);
any doSub(Context*, any ex);
any doMul(Context*, any ex);



uword length(any x);
/* Prototypes */
void *alloc(void*,size_t);
any apply(any,any,bool,int,cell*);
void argError(any,any) ;
void atomError(any,any) ;
void begString(void);
void brkLoad(any);
int bufNum(char[BITS/2],word);
int bufSize(any);
void bufString(any,char*);
void bye(int) ;
void pairError(any,any) ;
any circ(any);
word compare(any,any);
void newline(void);
any endString(void);
bool equal(any,any);
void err(any,any,char*,...) ;
word evNum(any,any);
any evSym(any);
void execError(char*) ;
int firstByte(any);
any get(any,any);
int getByte(int*,uword*,any*);
int getByte1(int*,uword*,any*);
void giveup(char*) ;
any isIntern(any,any[2]);
any method(any);
any mkTxt(int);
any name(any);
any numToSym(any,int,int,int);
void outName(any);
void outNum(word);
void outString(char*);
void protError(any,any) ;
void put(any,any,any);
void putByte0(int *i, uword *p, any *q);
void putByte1(int,int*,uword*,any*);
void putStdout(int);
void space(void);
int symBytes(any);
void symError(any,any) ;
void undefined(any,any);
void unwind (catchFrame*);
word xNum(any,any);
any xSym(any);
any doPrin(Context*, any x);
void openErr(any ex, char *s);
void eofErr(void);

any doHide(Context*, any);

any doQuote(Context*, any x);
any doEq(Context*, any x);
any doIf(Context*, any x);
any doDe(Context*, any ex);


any doLine(Context*, any x) ;
any doVeryLongFunc(Context*, any x);
any doLongFunc(Context*, any x);
any doChar(Context*, any ex) ;
any doIn(Context*, any ex) ;
any doOut(Context*, any ex) ;
any doWhile(Context*, any x) ;
any doDo(Context*, any x);

extern Context CONTEXT;
extern Context *_CONTEXT_PTR;



//gc.c
any cons(Context *, any x, any y);
/* Construct a symbol */
any consSym(Context *, any val, uword w);
/* Construct a name cell */
any consName(Context*, uword w, any n);
any consIntern(Context*, any x, any y);

any mkNum(Context *, word n);
any symToNum(Context*, any sym, int scl, int sep, int ign);

any read1(Context *, int);

any load(Context *, any,int,any);
any loadAll(Context *, any);

bool eol(Context *);
int pathSize(Context *, any);
void pathString(Context*, any,char*);

void wrOpen(Context*, any,any,outFrame*);
void rdOpen(Context*, any,any,inFrame*);

void pushInFiles(Context*, inFrame*);
void pushOutFiles(Context*, outFrame*);
void popInFiles(Context*);
void popOutFiles(Context*);

void redefine(Context*, any ex, any s, any x);
void redefMsg(Context *, any x, any y);

void getStdin(Context *);
void comment(Context*);

void heapAlloc(Context *);
uword getHeapSize(Context *);

any intern(Context*, any,any[2]);

void prin(Context *, any);
void print(any);

any evExpr(Context *, any,any);

any evList(Context *, any);
any EVAL(Context *, any x);

any prog(Context*, any x);
any run(Context*, any x);


any mkChar(Context *, int);

void putByte(Context *,int,int*,uword*,any*,cell*);
any popSym(Context *, int,uword,any,cell*);
any mkSym(Context *, byte*);
any mkStr(Context *, char*);
void pack(Context *, any,int*,uword*,any*,cell*);

void printTXT(Context *, any);
void printLongTXT(Context *, any);
#endif
