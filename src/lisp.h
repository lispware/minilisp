#ifndef __LISP_H__
#define __LISP_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <setjmp.h>

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

typedef any (*FunPtr)(any);

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
   void (*get)(void);
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
   void (*get)(void);
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
#define Save(c)         ((c).cdr=Env.stack, Env.stack=&(c))
#define drop(c)         (Env.stack=(c).cdr)
#define Push(c,x)       (data(c)=(x), Save(c))
#define Pop(c)          (drop(c), data(c))

#define Bind(s,f)       ((f).i=0, (f).cnt=1, (f).bnd[0].sym=(s), (f).bnd[0].val=val(s), (f).link=Env.bind, Env.bind=&(f))
#define Unbind(f)       (val((f).bnd[0].sym)=(f).bnd[0].val, Env.bind=(f).link)

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

any evList(any);
any EVAL(any x);
void lstError(any,any) ;
any consIntern(any x, any y);
/* Construct a cell */
any cons(any x, any y);

/* Construct a symbol */
any consSym(any val, uword w);

/* Construct a name cell */
any consName(uword w, any n);

#define evSubr(f,x)     (*(FunPtr)(num(f)))(x)

/* Globals */
extern int Chr, Trace;
extern char **AV, *AV0, *Home;
extern heap *Heaps;
extern cell *Avail;
extern stkEnv Env;
extern catchFrame *CatchPtr;
extern FILE *InFile, *OutFile;
extern any TheKey, TheCls, Thrown;
extern any Intern[2], Transient[2];
extern any ApplyArgs, ApplyBody;

#endif
