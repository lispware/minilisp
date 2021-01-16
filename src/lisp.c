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

#define WORD ((int)sizeof(long long))
#define BITS (8*WORD)

typedef signed long long word;
typedef unsigned long long uword;
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

CellPartType getCARType(any cell)
{
    return cell->type.parts[0];
}

CellPartType getCDRType(any cell)
{
    return cell->type.parts[1];
}

void setCARType(any cell, CellPartType type)
{
    if (!cell) return;
    cell->type.parts[0] = type;
}

void setCDRType(any cell, CellPartType type)
{
    if (!cell) return;
    cell->type.parts[1] = type;
}

void setList(any cell)
{
    cell->type.parts[2] = 1;
}

int isList(any cell)
{
    return cell->type.parts[2];
}

void setMark(any cell, int m)
{
    cell->type.parts[3] = m;
}

int getMark(any cell)
{
    return cell->type.parts[3];
}

#include "def.d"
#include "mem.d"

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

/*** Macros ***/
#define Free(p)         ((p)->car=Avail, (p)->cdr=0, (p)->type._t=0,  Avail=(p))

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

#define evSubr(f,x)     (*(FunPtr)(num(f)))(x)

/* Error checking */
#define NeedNum(ex,x)   if (!isNum(x)) numError(ex,x)
#define NeedSym(ex,x)   if (!isSym(x)) symError(ex,x)
#define NeedPair(ex,x)  if (!isCell(x)) pairError(ex,x)
#define NeedAtom(ex,x)  if (isCell(x)) atomError(ex,x)
#define NeedLst(ex,x)   if (!isCell(x) && !isNil(x)) lstError(ex,x)
#define NeedVar(ex,x)   if (isNum(x)) varError(ex,x)

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

void printTXT(any);
void printLongTXT(any);
static void gc(long long c);
uword getHeapSize(void);
/* Prototypes */
void *alloc(void*,size_t);
any apply(any,any,bool,int,cell*);
void argError(any,any) ;
void atomError(any,any) ;
void begString(void);
void brkLoad(any);
int bufNum(char[BITS/2],long long);
int bufSize(any);
void bufString(any,char*);
void bye(int) ;
void pairError(any,any) ;
any circ(any);
long long compare(any,any);
any consIntern(any,any);
any cons(any,any);
any consName(uword,any);
any consSym(any,uword);
void newline(void);
any endString(void);
bool equal(any,any);
void err(any,any,char*,...) ;
any evExpr(any,any);
any evList(any);
long long evNum(any,any);
any evSym(any);
void execError(char*) ;
int firstByte(any);
any get(any,any);
int getByte(int*,uword*,any*);
int getByte1(int*,uword*,any*);
void getStdin(void);
void giveup(char*) ;
void heapAlloc(void);
any intern(any,any[2]);
any isIntern(any,any[2]);
void lstError(any,any) ;
any load(any,int,any);
any loadAll(any);
any method(any);
any mkChar(int);
any mkSym(byte*);
any mkStr(char*);
any mkTxt(int);
any name(any);
void numError(any,any) ;
any numToSym(any,int,int,int);
void outName(any);
void outNum(long long);
void outString(char*);
void pack(any,int*,uword*,any*,cell*);
int pathSize(any);
void pathString(any,char*);
void popInFiles(void);
void popOutFiles(void);
any popSym(int,uword,any,cell*);
void prin(any);
void print(any);
void protError(any,any) ;
void pushInFiles(inFrame*);
void pushOutFiles(outFrame*);
void put(any,any,any);
void putByte(int,int*,uword*,any*,cell*);
void putByte1(int,int*,uword*,any*);
void putStdout(int);
void rdOpen(any,any,inFrame*);
any read1(int);
void space(void);
int symBytes(any);
void symError(any,any) ;
any symToNum(any,int,int,int);
void undefined(any,any);
void unwind (catchFrame*);
void varError(any,any) ;
void wrOpen(any,any,outFrame*);
long long xNum(any,any);
any xSym(any);

/* List length calculation */
static inline uword length(any x)
{
   uword n;

   for (n = 0; x != Nil; x = cdr(x)) ++n;
   return n;
}

/* List interpreter */
static inline any prog(any x)
{
   any y;

   do
   {
      y = EVAL(car(x));
   }
   while (Nil != (x = cdr(x)));

   return y;
}

static inline any run(any x)
{
   any y;
   cell at;

   Push(at,val(At));
   do
   {
      y = EVAL(car(x));
   }
   while (isCell(x = cdr(x)));
   val(At) = Pop(at);
   return y;
}

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

///////////////////////////////////////////////
//               sym.c
///////////////////////////////////////////////

int firstByte(any s)
{
    int c;

    if (isNil(s)) return 0;
    c = (uword)s;
    return c & 127;
}

int getByte1(int *i, uword *p, any *q)
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

int getByte(int *i, uword *p, any *q)
{
    int c;

    if (*i == 0)
    {
        if (!*q)
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

any mkTxt(int c)
{
    return txt(c & 127);
}

any mkChar(int c)
{
   return consSym(NULL, c & 127);
}

void putByte1(int c, int *i, uword *p, any *q)
{
    *p = c & 127;
    *i = 8;
    *q = NULL;
}

void putByte(int c, int *i, uword *p, any *q, cell *cp)
{
    c = c & 127;
    int d = 8;

    if (*i != BITS)
        *p |= (uword)c << *i;

    if (*i + d  > BITS)
    {
        if (*q)
        {
            any x = consName(*p, Zero);
            setCARType(x, BIN);
            (*q)->cdr = x;
            *q = x;
        }
        else
        {
            any x = consSym(NULL, 0);
            setCARType(x, BIN_START);
            Push(*cp, x);
            any y = consName(*p, Zero);
            setCARType(y, BIN);
            (*cp).car->car = *q = y;

        }
        *p = c >> BITS - *i;
        *i -= BITS;
    }

    *i += d;
}

any popSym(int i, uword n, any q, cell *cp)
{
    if (q)
    {
        //val(q) = i <= (BITS-2)? box(n) : consName(n, Zero);
        q->cdr = consName(n, Nil);
        return Pop(*cp);
    }
    return consSym(NULL,n);
}

int symBytes(any x)
{
    int cnt = 0;
    uword w;

    if (isNil(x))
        return 0;

    if (isTxt(x))
    {
        w = (uword)(x->car);
        while (w)
        {
            ++cnt;
            w >>= 8;
        }
    }

    return cnt;
}

any isIntern(any nm, any tree[2])
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

any internBin(any sym, any tree[2])
{
    any nm, x, y, z;
    word n;

    x = tree[1];

    if (x == Nil)
    {
        tree[1] = consIntern(sym, Nil);
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
            cdr(x) = n < 0 ? consIntern(consIntern(sym, Nil), Nil) : consIntern(Nil, consIntern(sym, Nil));
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
                cadr(x) = consIntern(sym, Nil);
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
                cddr(x) = consIntern(sym, Nil);
                return sym;
            }
        }
    }
}

any intern(any sym, any tree[2])
{
   any nm, x;
   word n;


   if (getCARType(sym) == BIN_START) return internBin(sym, tree);

   nm = sym;

   x = tree[0];
   if (Nil == x)
   {
      tree[0] = consIntern(sym, Nil);
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
         cdr(x) = n < 0 ? consIntern(consIntern(sym, Nil), Nil) : consIntern(Nil, consIntern(sym, Nil));
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
            cadr(x) = consIntern(sym, Nil);
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
            cddr(x) = consIntern(sym, Nil);
            return sym;
         }
      }
   }
}

/* Get symbol name */
any name(any s)
{
   return s;
}

/* Make name */
any mkSym(byte *s)
{
    int i;
    uword w;
    cell c1, *p;

    putByte1(*s++, &i, &w, &p);
    while (*s)
    {
        putByte(*s++, &i, &w, &p, &c1);
    }
    return popSym(i, w, p, &c1);
}

/* Make string */
any mkStr(char *s)
{
   if (s && *s)
   {
      return mkSym((byte *)s);
   }
   else
   {
      return Nil;
   }
}

// (==== ['sym ..]) -> NIL
any doHide(any ex)
{
    // TODO - is this needed?
   printf("%p\n", ex);

   return Nil;
}

// (not 'any) -> flg
any doNot(any x) {
   any a;

   if (isNil(a = EVAL(cadr(x))))
      return T;
   val(At) = a;
   return Nil;
}

// (c...r 'lst) -> any
any doCar(any ex)
{
   any x = cdr(ex);
   x = EVAL(car(x));
   NeedLst(ex,x);
   return car(x);
}

any doCdr(any ex)
{
   any x = cdr(ex);
   x = EVAL(car(x));
   NeedLst(ex,x);
   return cdr(x);
}

any doCons(any x)
{
   any y;
   cell c1;

   x = cdr(x);
   Push(c1, y = cons(EVAL(car(x)),Nil));
   while (Nil != (cdr(x = cdr(x))))
   {
      y = cdr(y) = cons(EVAL(car(x)),Nil);
   }
   cdr(y) = EVAL(car(x));
   return Pop(c1);
}


// (setq var 'any ..) -> any
any doSetq(any ex)
{
    any x, y;

    x = cdr(ex);
    do
    {
        y = car(x),  x = cdr(x);
        NeedVar(ex,y);
        // CheckVar(ex,y); - TODO - what is this for?
        val(y) = EVAL(car(x));
    }
    while (Nil != (x = cdr(x)));

    return val(y);
}

static void makeError(any ex) {err(ex, NULL, "Not making");}

// (link 'any ..) -> any
any doLink(any x)
{
    any y;

    if (!Env.make)
    {
        makeError(x);
    }
    x = cdr(x);
    do
    {
        y = EVAL(car(x));
        Env.make = &cdr(*Env.make = cons(y, Nil));
    }
    while (Nil != (x = cdr(x)));
    return y;
}

// (make .. [(made 'lst ..)] .. [(link 'any ..)] ..) -> any
any doMake(any x)
{
    any *make, *yoke;
    cell c1;

    Push(c1, Nil);
    make = Env.make;
    yoke = Env.yoke;
    Env.make = Env.yoke = &data(c1);

    while (Nil != (x = cdr(x)))
    {
        if (isCell(car(x)))
        {
            evList(car(x));
        }
    }
    Env.yoke = yoke;
    Env.make = make;
    return Pop(c1);
}

///////////////////////////////////////////////
//               sym.c - END
///////////////////////////////////////////////


///////////////////////////////////////////////
//               io.c - START
///////////////////////////////////////////////

static any read0(bool);

static char Delim[] = " \t\n\r\"'(),[]`~{}";

static void openErr(any ex, char *s)
{
    err(ex, NULL, "%s open: %s", s, strerror(errno));
}

static void eofErr(void)
{
    err(NULL, NULL, "EOF Overrun");
}

/* Buffer size */
int bufSize(any x)
{
    return symBytes(x) + 1;
}

int pathSize(any x)
{
    int c = firstByte(x);

    if (c != '@'  &&  (c != '+'))
    {
        return bufSize(x);
    }
    if (!Home)
    {
        return symBytes(x);
    }
    return strlen(Home) + symBytes(x);
}

void bufString(any x, char *p)
{
    int c, i;
    uword w;

    if (!isNil(x))
    {
        for (x = name(x), c = getByte1(&i, &w, &x); c; c = getByte(&i, &w, &x))
        {
            if (c == '^')
            {
                if ((c = getByte(&i, &w, &x)) == '?')
                {
                    c = 127;
                }
                else
                {
                    c &= 0x1F;
                }
            }
            *p++ = c;
        }
    }
    *p = '\0';
}

void pathString(any x, char *p)
{
    int c, i;
    uword w;
    char *h;

    if ((c = getByte1(&i, &w, &x)) == '+')
    {
        *p++ = c,  c = getByte(&i, &w, &x);
    }
    if (c != '@')
    {
        while (*p++ = c)
        {
            c = getByte(&i, &w, &x);
        }
    }
    else
    {
        if (h = Home)
        {
            do
            {
                *p++ = *h++;
            }
            while (*h);
        }

        while (*p++ = getByte(&i, &w, &x));
    }
}

void rdOpen(any ex, any x, inFrame *f)
{
    //NeedSymb(ex,x); // TODO WHAT IS THIS ABOUT?
    if (isNil(x))
    {
        f->fp = stdin;
    }
    else
    {
        int ps = pathSize(x);
        char *nm = (char*)malloc(ps);

        pathString(x,nm);
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

/*** Reading ***/
void getStdin(void)
{
    Chr = getc(InFile);
}

void pushInFiles(inFrame *f)
{
    f->next = Chr,  Chr = 0;
    InFile = f->fp;
    f->get = Env.get,  Env.get = getStdin;
    f->link = Env.inFrames,  Env.inFrames = f;
}

void pushOutFiles(outFrame *f)
{
    OutFile = f->fp;
    f->put = Env.put,  Env.put = putStdout;
    f->link = Env.outFrames,  Env.outFrames = f;
}

void popInFiles(void)
{
    if (InFile != stdin)
    {
        fclose(InFile);
    }
    Chr = Env.inFrames->next;
    Env.get = Env.inFrames->get;
    InFile = (Env.inFrames = Env.inFrames->link)?  Env.inFrames->fp : stdin;
}

void popOutFiles(void)
{
    if (OutFile != stdout && OutFile != stderr)
    {
        fclose(OutFile);
    }
    Env.put = Env.outFrames->put;
    OutFile = (Env.outFrames = Env.outFrames->link)? Env.outFrames->fp : stdout;
}

/* Skip White Space and Comments */
static int skipc(int c)
{
    if (Chr < 0)
    {
        return Chr;
    }
    for (;;)
    {
        while (Chr <= ' ')
        {
            Env.get();
            if (Chr < 0)
            {
                return Chr;
            }
        }
        if (Chr != c)
        {
            return Chr;
        }
        Env.get();
        while (Chr != '\n')
        {
            if (Chr < 0)
            {
                return Chr;
            }
            Env.get();
        }
    }
}

static void comment(void)
{
    Env.get();
    if (Chr != '{')
    {
        while (Chr != '\n')
        {
            if (Chr < 0)
            {
                return;
            }
            Env.get();
        }
    }
    else
    {
        int n = 0;

        for (;;) {  // #{block-comment}# from Kriangkrai Soatthiyanont
            Env.get();
            if (Chr < 0)
            {
                return;
            }
            if (Chr == '#'  &&  (Env.get(), Chr == '{'))
            {
                ++n;
            }
            else if (Chr == '}'  &&  (Env.get(), Chr == '#')  &&  --n < 0)
            {
                break;
            }
        }
        Env.get();
    }
}

static int skip(void)
{
    for (;;)
    {
        if (Chr < 0)
        {
            return Chr;
        }
        while (Chr <= ' ')
        {
            Env.get();
            if (Chr < 0)
            {
                return Chr;
            }
        }

        if (Chr != '#')
        {
            return Chr;
        }
        comment();
    }
}

/* Test for escaped characters */
static bool testEsc(void)
{
    for (;;)
    {
        if (Chr < 0)
            return NO;
        if (Chr != '\\')
            return YES;
        if (Env.get(), Chr != '\n')
            return YES;
        do
        {
            Env.get();
        }
        while (Chr == ' '  ||  Chr == '\t');
    }
}

/* Read a list */
static any rdList(void)
{
    any x;
    cell c1;

    for (;;)
    {
        if (skip() == ')')
        {
            Env.get();
            return Nil;
        }
        if (Chr == ']')
        {
            return Nil;
        }
        if (Chr != '~')
        {
            x = cons(read0(NO),Nil);
            Push(c1, x);
            break;
        }
        Env.get();

        x = read0(NO);
        Push(c1, x);
        if (isCell(x = data(c1) = EVAL(data(c1))))
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
        if (skip() == ')')
        {
            Env.get();
            break;
        }
        if (Chr == ']')
            break;
        if (Chr == '.')
        {
            Env.get();
            cdr(x) = skip()==')' || Chr==']'? data(c1) : read0(NO);
            if (skip() == ')')
                Env.get();
            else if (Chr != ']')
                err(NULL, x, "Bad dotted pair");
            break;
        }
        if (Chr != '~')
        {
            x = cdr(x) = cons(read0(NO),Nil);
        }
        else
        {
            Env.get();
            cdr(x) = read0(NO);
            cdr(x) = EVAL(cdr(x));
            while (isCell(cdr(x)))
            {
                x = cdr(x);
            }
        }
    }
    return Pop(c1);
}

/* Read one expression */
static any read0(bool top)
{
    int i;
    uword w;
    any x, y;
    cell c1, *p;

    if (skip() < 0)
    {
        if (top)
            return Nil;
        eofErr();
    }
    if (Chr == '(')
    {
        Env.get();
        x = rdList();
        if (top  &&  Chr == ']')
            Env.get();
        return x;
    }
    if (Chr == '[')
    {
        Env.get();
        x = rdList();
        if (Chr != ']')
            err(NULL, x, "Super parentheses mismatch");
        Env.get();
        return x;
    }
    if (Chr == '\'')
    {
        Env.get();
        return cons(doQuote_D, read0(top));
    }
    if (Chr == ',')
    {
        Env.get();
        return read0(top);
    }
    if (Chr == '`')
    {
        Env.get();
        Push(c1, read0(top));
        x = EVAL(data(c1));
        drop(c1);
        return x;
    }
    if (Chr == '"')
    {
        Env.get();
        if (Chr == '"')
        {
            Env.get();
            return Nil;
        }
        if (!testEsc())
            eofErr();
        putByte1(Chr, &i, &w, &p);
        while (Env.get(), Chr != '"')
        {
            if (!testEsc())
                eofErr();
            putByte(Chr, &i, &w, &p, &c1);
        }
        y = popSym(i, w, p, &c1),  Env.get();
        if (x = isIntern(tail(y), Transient))
            return x;
        intern(y, Transient);
        return y;
    }
    if (strchr(Delim, Chr))
        err(NULL, NULL, "Bad input '%c' (%d)", isprint(Chr)? Chr:'?', Chr);
    if (Chr == '\\')
        Env.get();
    putByte1(Chr, &i, &w, &p);

    int count=0;
    for (;;)
    {
        count++;
        // if (count > 6)
        // {
        //     printf("%s too long\n", (char*)&w);
        //     bye(0);
        // }
        Env.get();
        if (strchr(Delim, Chr))
        {
            break;
        }
        if (Chr == '\\')
        {
            Env.get();
        }
        putByte(Chr, &i, &w, &p, &c1);
    }

    y = popSym(i, w, p, &c1);
    //printf("%p --> CAR = %p CDR = %p \n", y, y->car, y->cdr);
    if (x = symToNum(tail(y), 0, '.', 0))
    {
        return x;
    }
    if (x = isIntern(tail(y), Intern))
    {
        return x;
    }

    intern(y, Intern);
    //val(y) = Nil;
    return y;
}

any read1(int end)
{
   if (!Chr)
      Env.get();
   if (Chr == end)
      return Nil;
   return read0(YES);
}

/* Read one token */
any token(any x, int c)
{
    int i;
    uword w;
    any y;
    cell c1, *p;

    if (!Chr)
        Env.get();
    if (skipc(c) < 0)
        return Nil;
    if (Chr == '"')
    {
        Env.get();
        if (Chr == '"')
        {
            Env.get();
            return Nil;
        }
        if (!testEsc())
            return Nil;
        Push(c1, y =  cons(mkChar(Chr), Nil));
        while (Env.get(), Chr != '"' && testEsc())
            y = cdr(y) = cons(mkChar(Chr), Nil);
        Env.get();
        return Pop(c1);
    }
    if (Chr >= '0' && Chr <= '9')
    {
        putByte1(Chr, &i, &w, &p);
        while (Env.get(), Chr >= '0' && Chr <= '9' || Chr == '.')
            putByte(Chr, &i, &w, &p, &c1);
        return symToNum(tail(popSym(i, w, p, &c1)), 0, '.', 0);
    }
    if (Chr != '+' && Chr != '-')
    {
        // TODO check what needs to be done about stack - FREE MUST BE ADDED
        // char nm[bufSize(x)];
        char *nm = (char *)malloc(bufSize(x));

        bufString(x, nm);
        if (Chr >= 'A' && Chr <= 'Z' || Chr == '\\' || Chr >= 'a' && Chr <= 'z' || strchr(nm,Chr))
        {
            if (Chr == '\\')
                Env.get();
            putByte1(Chr, &i, &w, &p);
            while (Env.get(),
                    Chr >= '0' && Chr <= '9' || Chr >= 'A' && Chr <= 'Z' ||
                    Chr == '\\' || Chr >= 'a' && Chr <= 'z' || strchr(nm,Chr) )
            {
                if (Chr == '\\')
                    Env.get();
                putByte(Chr, &i, &w, &p, &c1);
            }
            y = popSym(i, w, p, &c1);
            if (x = isIntern(tail(y), Intern))
            {
                free(nm);
                return x;
            }
            intern(y, Intern);
            val(y) = Nil;
            free(nm);
            return y;
        }
    }
    y = mkTxt(c = Chr);
    Env.get();
    return mkChar(c);
}

static inline bool eol(void)
{
   if (Chr < 0)
      return YES;
   if (Chr == '\n')
   {
      Chr = 0;
      return YES;
   }
   if (Chr == '\r')
   {
      Env.get();
      if (Chr == '\n')
         Chr = 0;
      return YES;
   }
   return NO;
}

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
        if (InFile != stdin)
        {
            data(c1) = read1(0);
        }
        else
        {
            if (pr && !Chr)
                Env.put(pr), space(), fflush(OutFile);
            data(c1) = read1('\n');
            while (Chr > 0)
            {
                if (Chr == '\n')
                {
                    Chr = 0;
                    break;
                }
                if (Chr == '#')
                    comment();
                else
                {
                    if (Chr > ' ')
                        break;
                    Env.get();
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
        if (InFile != stdin || Chr || !pr)
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
            outString("-> "),  fflush(OutFile),  print(x),  newline();
        }
        drop(c1);
    }
}

/*** Prining ***/
void putStdout(int c)
{
    putc(c, OutFile);
}

void newline(void)
{
    Env.put('\n');
}

void space(void)
{
    Env.put(' ');
}

void outString(char *s)
{
    while (*s)
        Env.put(*s++);
}

int bufNum(char buf[BITS/2], long long n)
{
    return sprintf(buf, "%lld", n); // TODO - this is not quite right for 32 bit
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
    if (getCARType(x) == TXT)
    {
        printf("%s", (char*)&x->car);
        return;
    }
    if (getCARType(x) == BIN_START)
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
                    Env.put(c);
                else if (!(c = getByte(&i, &w, &x)))
                    Env.put('^');
                else if (c == '?')
                    Env.put(127);
                else
                    Env.put(c &= 0x1F);
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

// (prin 'any ..) -> any
any doPrin(any x)
{
   any y = Nil;

   while (Nil != (x = cdr(x)))
   {
      prin(y = EVAL(car(x)));
   }
   newline();
   return y;
}

///////////////////////////////////////////////
//               io.c - END
///////////////////////////////////////////////


///////////////////////////////////////////////
//               math.c START
///////////////////////////////////////////////


/* Make number from symbol */
any symToNum(any sym, int scl, int sep, int ign)
{
    unsigned c;
    int i;
    uword w;
    bool sign, frac;
    long long n;
    any s = sym;


    if (!(c = getByte1(&i, &w, &s)))
        return NULL;

    while (c <= ' ')  /* Skip white space */
    {
        if (!(c = getByte(&i, &w, &s)))
            return NULL;
    }

    sign = NO;
    if (c == '+'  ||  c == '-' && (sign = YES))
    {
        if (!(c = getByte(&i, &w, &s)))
            return NULL;
    }

    if ((c -= '0') > 9)
        return NULL;

    frac = NO;
    n = c;
    while ((c = getByte(&i, &w, &s))  &&  (!frac || scl))
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
        while (c = getByte(&i, &w, &s))
        {
            if ((c -= '0') > 9)
                return NULL;
        }
    }
    if (frac)
        while (--scl >= 0)
            n *= 10;


    any r = cons(Nil, Nil);
    r->car = (any)n;
    r->type.parts[0] = NUM;

    return r;
}

// (+ 'num ..) -> num
any doAdd(any ex)
{
    any x, y;
    uword n=0;

    x = cdr(ex);
    if (isNil(y = EVAL(car(x))))
        return Nil;
    NeedNum(ex,y);
    n = unBox(y);
    while (Nil != (x = cdr(x)))
    {
        if (isNil(y = EVAL(car(x))))
            return Nil;
        NeedNum(ex,y);
        n += unBox(y);
    }

    any r = cons(Nil, Nil);
    r->car = (any)n;
    r->type.parts[0] = NUM;
    return r;
}

any doSub(any ex)
{
    any x, y;
    uword n=0;

    x = cdr(ex);
    if (isNil(y = EVAL(car(x))))
        return Nil;
    NeedNum(ex,y);
    n = unBox(y);
    while (Nil != (x = cdr(x)))
    {
        if (isNil(y = EVAL(car(x))))
            return Nil;
        NeedNum(ex,y);
        n -= unBox(y);
    }

    any r = cons(Nil, Nil);
    r->car = (any)n;
    r->type.parts[0] = NUM;
    return r;
}

any doMul(any ex)
{
    any x, y;
    uword n=0;

    x = cdr(ex);
    if (isNil(y = EVAL(car(x))))
        return Nil;
    NeedNum(ex,y);
    n = unBox(y);
    while (Nil != (x = cdr(x)))
    {
        if (isNil(y = EVAL(car(x))))
            return Nil;
        NeedNum(ex,y);
        n *= unBox(y);
    }

    any r = cons(Nil, Nil);
    r->car = (any)n;
    r->type.parts[0] = NUM;
    return r;
}

///////////////////////////////////////////////
//               math.c END
///////////////////////////////////////////////


///////////////////////////////////////////////
//               flow.c START
///////////////////////////////////////////////

static void redefMsg(any x, any y)
{
   FILE *oSave = OutFile;

   OutFile = stderr;
   outString("# ");
   print(x);
   if (y)
      space(), print(y);
   outString(" redefined\n");
   OutFile = oSave;
}

static void redefine(any ex, any s, any x)
{
   //NeedSymb(ex,s); TODO - GOTTA KNOW WHAT"S GOING ON HERE
   //CheckVar(ex,s);

   if (ex == Nil)
   {
      giveup("THIS SHOULD NOT HAPPEN");
   }

   // TODO bring back redifine message perhaps?
   //if (!isNil(val(s))  &&  s != val(s)  &&  !equal(x,val(s)))
   if (!isNil(val(s))  &&  s != val(s))
      redefMsg(s,NULL);
   val(s) = x;

   setCDRType(s, PTR_CELL); // TODO - DO IT MORE NEATLY
}

// (quote . any) -> any
any doQuote(any x)
{
    return cdr(x);
}

// (== 'any ..) -> flg
any doEq(any x)
{
   cell c1;

   x = cdr(x),  Push(c1, EVAL(car(x)));
   while (Nil != (x = cdr(x)))
   {
      //if (data(c1) != EVAL(car(x))) { // TODO CHECK IT OUT
      if (car(data(c1)) != car(EVAL(car(x))))
      {
         drop(c1);
         return Nil;
      }
   }
   drop(c1);
   return T;
}

// (if 'any1 any2 . prg) -> any
any doIf(any x)
{
   any a;

   x = cdr(x);
   if (isNil(a = EVAL(car(x))))
      return prog(cddr(x));
   val(At) = a;
   x = cdr(x);
   return EVAL(car(x));
}

// (de sym . any) -> sym
any doDe(any ex)
{
   redefine(ex, cadr(ex), cddr(ex));
   return cadr(ex);
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
any doLet(any x)
{
    any y;

    x = cdr(x);
    if (!isCell(y = car(x)))
    {
        bindFrame f;

        x = cdr(x),  Bind(y,f),  val(y) = EVAL(car(x));
        x = prog(cdr(x));
        Unbind(f);
    }
    else
    {
        // TODO check out how to do stack 
        bindFrame *f = allocFrame((length(y)+1)/2);

        f->link = Env.bind,  Env.bind = f;
        f->i = f->cnt = 0;
        do
        {
            f->bnd[f->cnt].sym = car(y);
            f->bnd[f->cnt].val = val(car(y));
            ++f->cnt;
            val(car(y)) = EVAL(cadr(y));
        }
        while (isCell(y = cddr(y)) && y != Nil);
        x = prog(cdr(x));
        while (--f->cnt >= 0)
            val(f->bnd[f->cnt].sym) = f->bnd[f->cnt].val;
        Env.bind = f->link;

        free(f);
    }
    return x;
}

// (bye 'num|NIL)
any doBye(any ex)
{
   printf("\n");
   bye(0);
   return ex;
}


void wrOpen(any ex, any x, outFrame *f) {
   //NeedSymb(ex,x);
   if (isNil(x))
      f->fp = stdout;
   else {
      char *nm = (char *)malloc(pathSize(x));

      pathString(x,nm);
      if (nm[0] == '+') {
         if (!(f->fp = fopen(nm+1, "a")))
            openErr(ex, nm);
      }
      else if (!(f->fp = fopen(nm, "w")))
         openErr(ex, nm);

      free(nm);
   }
}


// (line 'flg) -> lst|sym
any doLine(any x) {
   any y;
   int i;
   word w;
   cell c1;

   if (!Chr)
      Env.get();
   if (eol())
      return Nil;
   x = cdr(x);
   if (isNil(EVAL(car(x)))) {
      Push(c1, cons(mkChar(Chr), Nil));
      y = data(c1);
      for (;;) {
         if (Env.get(), eol())
            return Pop(c1);
         y = cdr(y) = cons(mkChar(Chr), Nil);
      }
   }
   else {
      putByte1(Chr, &i, &w, &y);
      for (;;) {
         if (Env.get(), eol())
            return popSym(i, w, y, &c1);
         putByte(Chr, &i, &w, &y, &c1);
      }
   }
}


any doLongFunc(any x)
{
    printf("LONG FUNCTION CALLED\n");
    return x;
}

// (char) -> sym
// (char 'num) -> sym
// (char 'sym) -> num
any doChar(any ex) {
   any x = cdr(ex);

   if (x == Nil) {
      if (!Chr)
         Env.get();
      x = Chr<0? Nil : mkChar(Chr);
      Env.get();
      return x;
   }
   // TODO - fix this up
   // if (isNum(x = EVAL(car(x)))) {
   //    int c = (int)unBox(x);

   //    ////if (c == 0)
   //    ////   return Nil;
   //    ////if (c == 127)
   //    ////   return mkChar2('^','?');
   //    ////if (c < ' ')
   //    ////   return mkChar2('^', c + 0x40);
   //    return mkChar(c);
   // }
   // if (isSym(x)) {
   //    int c;

   //    if ((c = firstByte(x)) != '^')
   //       return box(c);
   //    return box((c = secondByte(x)) == '?'? 127 : c & 0x1F);
   // }
   atomError(ex,x);
}

any doIn(any ex) {
   any x;
   inFrame f;

   x = cdr(ex),  x = EVAL(car(x));
   rdOpen(ex,x,&f);
   pushInFiles(&f);
   x = prog(cddr(ex));
   popInFiles();
   return x;
}

// (out 'any . prg) -> any
any doOut(any ex) {
   any x;
   outFrame f;

   x = cdr(ex),  x = EVAL(car(x));
   wrOpen(ex,x,&f);
   pushOutFiles(&f);
   x = prog(cddr(ex));
   popOutFiles();
   return x;
}

// (while 'any . prg) -> any
any doWhile(any x) {
   any cond, a;
   cell c1;

   cond = car(x = cdr(x)),  x = cdr(x);
   Push(c1, Nil);
   while (!isNil(a = EVAL(cond))) {
      val(At) = a;
      data(c1) = prog(x);
   }
   return Pop(c1);
}

// (do 'flg|num ['any | (NIL 'any . prg) | (T 'any . prg) ..]) -> any
any doDo(any x)
{
    any f, y, z, a;
    word N=-1;

    x = cdr(x);
    if (isNil(f = EVAL(car(x))))
        return Nil;
    if (isNum(f) && num(f) < 0)
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
                    if (isNil(a = EVAL(car(z))))
                        return prog(cdr(z));
                    val(At) = a;
                    z = Nil;
                }
                else if (car(z) == T)
                {
                    z = cdr(z);
                    if (!isNil(a = EVAL(car(z))))
                    {
                        val(At) = a;
                        return prog(cdr(z));
                    }
                    z = Nil;
                }
                else
                    z = evList(z);
            }
        } while (Nil != (y = cdr(y)));
    }
}

///////////////////////////////////////////////
//               flow.c END
///////////////////////////////////////////////


///////////////////////////////////////////////
//               gc.c START
///////////////////////////////////////////////


static void mark(any x);
static void mark(any x)
{
    if (!x) return;

    if (getMark(x)) return;

    setMark(x, 1);

    if (x == Nil) return;

    if (getCARType(x) == BIN_START)
    {
        mark(cdr(x));
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
   if(free)Avail = NULL;
   h = Heaps;
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

   printf("AVAIL = %p\n", Avail);
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
   setMark(Intern[0], 0);mark(Intern[0]);
   setMark(Intern[1], 0);mark(Intern[1]);
   setMark(Transient[0], 0);mark(Transient[0]);
   setMark(Transient[1], 0);mark(Transient[1]);
   if (ApplyArgs) setMark(ApplyArgs, 0);mark(ApplyArgs);
   if (ApplyBody) setMark(ApplyBody, 0);mark(ApplyBody);
   for (p = Env.stack; p; p = cdr(p))
   {
      mark(car(p));
   }
   for (p = (any)Env.bind;  p;  p = (any)((bindFrame*)p)->link)
   {
      for (i = ((bindFrame*)p)->cnt;  --i >= 0;)
      {
         mark(((bindFrame*)p)->bnd[i].sym);
         mark(((bindFrame*)p)->bnd[i].val);
      }
   }
   for (p = (any)CatchPtr; p; p = (any)((catchFrame*)p)->link)
   {
      if (((catchFrame*)p)->tag)
         mark(((catchFrame*)p)->tag);
      mark(((catchFrame*)p)->fin);
   }
}

any doHS(any ignore)
{
    getHeapSize();
    return ignore;
}

any doDump(any ignore)
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
    //     gc(CELLS);
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

    heap *h = Heaps;

    dumpHeaps(mem, h);

    fclose(mem);

    return Nil;
}


uword getHeapSize(void)
{
    uword size = 0;
    uword sizeFree = 0;
    heap *h = Heaps;
    do
    {
        any p = h->cells + CELLS-1;
        do
        {
            size++;
        }
        while (--p >= h->cells);
    } while (h = h->next);

    any p = Avail;
    while (p)
    {
        sizeFree++;
        p = car(p);
    }

    printf("MEM SIZE = %lld FREE = %lld Nil = %p\n", size, sizeFree, Nil);

    return size;
}

/* Garbage collector */
static void gc(long long c)
{
    any p;
    heap *h;

    doDump(Nil);
    markAll();
    doDump(Nil);

    /* Sweep */
    Avail = NULL;
    h = Heaps;
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

    doDump(Nil);
    return;
}

any consIntern(any x, any y)
{
    any r = cons(x, y);

    setCARType(r, INTERN);
    setCDRType(r, INTERN);

    return r;
}

/* Construct a cell */
any cons(any x, any y)
{
    cell *p;

    if (!(p = Avail))
    {
        cell c1, c2;

        Push(c1,x);
        Push(c2,y);
        gc(CELLS);
        drop(c1);
        p = Avail;
    }
    Avail = p->car;
    p->car = x;
    p->cdr = y;
    setCARType(p, PTR_CELL);
    setCDRType(p, PTR_CELL);
    return p;
}

/* Construct a symbol */
any consSym(any val, uword w)
{
    cell *p;

    if (!(p = Avail)) {
        cell c1;

        if (!val)
            gc(CELLS);
        else {
            Push(c1,val);
            gc(CELLS);
            drop(c1);
        }
        p = Avail;
    }
    Avail = p->car;
    p->cdr = val ? val : p;
    p->car = (any)w;
    setCARType(p, TXT);
    setCDRType(p, PTR_CELL);
    return p;
}

/* Construct a name cell */
any consName(uword w, any n)
{
   cell *p;

   if (!(p = Avail))
   {
      gc(CELLS);
      p = Avail;
   }
   Avail = p->car;
   p = symPtr(p);
   p->car = (any)w;
   p->cdr = n;
   setCARType(p, TXT);
   setCDRType(p, PTR_CELL);
   return p;
}
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

/* Allocate cell heap */
void heapAlloc(void)
{
   heap *h;
   cell *p;

   h = (heap*)((long long)alloc(NULL, sizeof(heap) + sizeof(cell)) + (sizeof(cell)-1) & ~(sizeof(cell)-1));
   h->next = Heaps,  Heaps = h;
   p = h->cells + CELLS-1;
   do
   {
      Free(p);
   }
   while (--p >= h->cells);
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

   f->link = Env.bind,  Env.bind = f;
   f->i = (bindSize * (length(y)+2)) / (2*sizeof(any)) - 1;
   f->cnt = 1,  f->bnd[0].sym = At,  f->bnd[0].val = val(At);
   while (y != Nil)
   {
      f->bnd[f->cnt].sym = car(y);
      f->bnd[f->cnt].val = EVAL(car(x));
      ++f->cnt, x = cdr(x), y = cdr(y);
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

      n = Env.next,  Env.next = cnt;
      arg = Env.arg,  Env.arg = c;
      x = prog(cdr(expr));
      if (cnt)
      {
         drop(c[cnt-1]);
      }

      Env.arg = arg,  Env.next = n;
      free(c);
   }

   while (--f->cnt >= 0)
   {
      val(f->bnd[f->cnt].sym) = f->bnd[f->cnt].val;
   }

   Env.bind = f->link;
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

   while (*AV  &&  strcmp(*AV,"-") != 0)
      x = load(ex, 0, mkStr(*AV++));
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
            Env.put('\\');
        }
        Env.put(c);
    }
   while (c = getByte(&i, &w, &nm));
}

void printTXT(any cell)
{
    printLongTXT(cell);
}

void printNUM(any cell)
{
    printf("%lld", (long long)cell->car);
}

void printCell(any cell)
{
    if (cell == Nil)
    {
        printf("Nil");
        return;
    }

    CellPartType carType = getCARType(cell);

    if (carType == TXT)
    {
        printTXT(cell);
    }
    else if (carType == NUM)
    {
        printNUM(cell);
    }

    if (isList(cell))
    {
        printf("(");
        while(isList(cell))
        {
            carType = getCARType(cell);
            if (carType == TXT) printTXT(cell);
            else if (carType == NUM) printNUM(cell);
            else printCell(cell->car);
            cell = cell->cdr;
            printf(" ");
        }
        printf(")");
    }
}

/*** Main ***/
int main(int ac, char *av[])
{
   if (ac == 0) printf("STRANGE\n");

   av++;
   AV = av;
   heapAlloc();
   doDump(Nil);
   //getHeapSize();
   //CELLS = 1;
   Intern[0] = Intern[1] = Transient[0] = Transient[1] = Nil;

   Mem[4] = (any)Mem; // TODO - SETTING THE VALUE OF NIL
   Mem[7] = (any)(Mem+6); // TODO - SETTING THE VALUE OF NIL

   for (int i = 3; i < MEMS; i += 3) // 2 because Nil has already been interned
   {
      any cell = (any)&Mem[i];
      CellPartType carType = getCARType(cell);
      CellPartType cdrType = getCDRType(cell);

      if ((BIN_START == carType || TXT == carType) && cdrType != FUNC && cell->cdr)
      {
         intern(cell, Intern);
      }
      else if ((BIN_START == carType || TXT == carType) && cdrType == FUNC && cell->cdr)
      {
         intern(cell, Intern);
      }
      else if ((BIN_START == carType || TXT == carType))
      {
         intern(cell, Intern);
      }
   }

   InFile = stdin, Env.get = getStdin;
   OutFile = stdout, Env.put = putStdout;
   ApplyArgs = cons(cons(consSym(Nil, 0), Nil), Nil);
   ApplyBody = cons(Nil, Nil);

   doDump(Nil);
   //getHeapSize();
   loadAll(NULL);
   while (!feof(stdin))
      load(NULL, ':', Nil);
   bye(0);
}
