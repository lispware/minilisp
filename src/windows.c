#ifndef __LISP_H__
#define __LISP_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <setjmp.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <tommath.h>


/*******************************************************************************
DEFINITONS
*******************************************************************************/

/* Cell access */
#define makeptr(x)      ((any)((uword)(x) & ~3))
#define car(x)          ((makeptr(x))->car)
#define cdr(x)          ((makeptr(x))->cdr)
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

extern int PUSH_POP;
//void ppp(Context *, char*);

#define data(c)         ((c).car)
#define Save(c)         (PUSH_POP++, (c).cdr=CONTEXT_PTR->Env.stack, CONTEXT_PTR->Env.stack=&(c), ppp(CONTEXT_PTR, "save\n", c))
#define drop(c)         (PUSH_POP--,  CONTEXT_PTR->Env.stack=(c).cdr, ppp(CONTEXT_PTR, "drop\n", c), CONTEXT_PTR->Env.stack)
#define Push(c,x)       (data(c)=(x), Save(c))
#define Pop(c)          (drop(c), data(c))

#define Bind(s,f)       ((f).i=0, (f).cnt=1, (f).bnd[0].sym=(s), (f).bnd[0].val=val(s), (f).link=CONTEXT_PTR->Env.bind, CONTEXT_PTR->Env.bind=&(f))
#define Unbind(f)       (val((f).bnd[0].sym)=(f).bnd[0].val, CONTEXT_PTR->Env.bind=(f).link)

#define Nil (&(CONTEXT_PTR->Mem[0]))
#define T (&(CONTEXT_PTR->Mem[3]))
#define At (&(CONTEXT_PTR->Mem[5]))
#define At2 (&(CONTEXT_PTR->Mem[7]))
#define At3 (&(CONTEXT_PTR->Mem[9]))
#define doQuote_D (&(CONTEXT_PTR->Mem[11]))

#ifndef CELLS
#define CELLS (1024)
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
}
cell, *any;

struct _Context;

typedef any (*FunPtr)(struct _Context *, any);

typedef enum
{
    FUNC, // this needs to be at 0; look at copyFixup where its used
    PTR_CELL,
    BIN,
    EXT,
} CellPartType;

#define BIN_START PTR_CELL

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

    // DEBUG
    int Tab;
    int THREAD_COUNT;
    word THREAD_ID;

}
Context;

typedef enum
{
    EXT_SOCKET,
    EXT_NUM,
} EXT_TYPE;

typedef struct _external
{
    EXT_TYPE type;
    void *pointer;
    void (*release)(struct _external*);
    char *(*print)(Context *, struct _external*);
    int (*equal)(Context *, struct _external *, struct _external *);
    struct _external *(*copy)(Context *, struct _external *);
} external;


#if 0
#define GetType(x) (((any)(x))->meta.type.parts[0])
#define setCARType(C, V) ((C)->meta.type.parts[0] = V)
#define setPtrType(x, T)  x
#else
#define setPtrType(x, T) ((any)((((uword)x) & ~3) | T))
#define GetType(x) ((uword)(cdr(x)) & 3)
#define setCARType(C, V) (cdr(C) = (any)(((uword)(cdr(C)) & ~3) | V))
#endif

/* Predicates */
#define isNil(x)        (makeptr(x)==Nil)
#define isCell(x)        ((GetType(x) == PTR_CELL) && GetType(car(x)) != BIN)
#define isFunc(x)        (GetType(x) == FUNC)
#define isSym(x)        ((GetType(x) == PTR_CELL) && GetType(car(x)) == BIN)
#define isNum(x)        ((GetType(x) == EXT) && (((external*)(((any)car(x))))->type == EXT_NUM))

#define NewNumber(EXTRA_PARAM, MATH_NUM, R)  external *EXTRA_PARAM = (external *)malloc(sizeof(external));\
                                EXTRA_PARAM->type = EXT_NUM;\
                                EXTRA_PARAM->release = releaseExtNum;\
                                EXTRA_PARAM->print = printExtNum;\
                                EXTRA_PARAM->copy = copyExtNum;\
                                EXTRA_PARAM->equal = equalExtNum;\
                                EXTRA_PARAM->pointer = (void*)(MATH_NUM);\
                                any R = cons(CONTEXT_PTR, Nil, Nil);\
                                car(R) = (any)EXTRA_PARAM;\
                                setCARType(R, EXT);


/* Error checking */
#define NeedNum(ex,x)   if (!isNum(x)) numError(ex,x)
// #define NeedSym(ex,x)   if (!isSym(x)) symError(ex,x)
// #define NeedPair(ex,x)  if (!isCell(x)) pairError(ex,x)
// #define NeedAtom(ex,x)  if (isCell(x)) atomError(ex,x)
#define NeedLst(ex,x)   if (!isCell(x) && !isNil(x)) lstError(ex,x)
#define NeedVar(ex,x)   if (isNum(x)) varError(ex,x)

#define num(x)          ((mp_int*)(((external*)((any)car(x)))->pointer))
#define tail(x)         (x)
#define val(x)          (cdr(x))
#define symPtr(x)       (x)
#define unBox(n)        (num(car(n)))

void lstError(any,any) ;
void numError(any,any) ;
void openErr(any ex, char *s);
/* Construct a cell */
#define evSubr(f,x)     (*(FunPtr)(((f))))(CONTEXT_PTR, x)



void debugIndent(Context *CONTEXT_PTR);
void debugOutdent(Context *CONTEXT_PTR);
void debugLog(Context *CONTEXT_PTR, char *message);
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
void rdOpen(Context *CONTEXT_PTR, any ex, any x, inFrame *f);
void popInFiles(Context *CONTEXT_PTR);
void pushOutFiles(Context *CONTEXT_PTR, outFrame *f);
void wrOpen(Context *CONTEXT_PTR, any ex, any x, outFrame *f);
any mkChar(Context *CONTEXT_PTR, int c);
any copyNum(Context *CONTEXT_PTR, any n);
void releaseExtNum(struct _external* obj);
char *printExtNum(Context *CONTEXT_PTR, struct _external* obj);
external * copyExtNum(Context *CONTEXT_PTR, external *ext);
int equalExtNum(Context *CONTEXT_PTR, external*x, external*y);
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
void printTree(Context *CONTEXT_PTR, any r, int tab, int t);

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
any consSym(Context *CONTEXT_PTR, any val, any w);
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
void *allignedAlloc(size_t size);
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
void dumpMemory(Context *CONTEXT_PTR, char *);
#define dump(x) dumpMemory(CONTEXT_PTR, x);

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
any doLength(Context *CONTEXT_PTR, any x);
any doList(Context *CONTEXT_PTR, any x);
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
any doCond(Context *CONTEXT_PTR, any x);
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
// THIS IS FROM lisp/builtin/net/windows/putStdoutNet.c

void putStdoutNet(Context *CONTEXT_PTR, int c)
{
    unsigned char buf[1];
    buf[0] = (unsigned char)c;
    SOCKET current_client = (SOCKET)CONTEXT_PTR->OutFile;

    send(current_client,buf,1,0);
}
// THIS IS FROM lisp/builtin/net/windows/pltClose.c

void pltClose(struct _external* obj)
{
    if (obj->type != EXT_SOCKET)
    {
        fprintf(stderr, "Not a socket\n");
        exit(0);
    }
    closesocket((int)obj->pointer);
    free(obj);
}
// THIS IS FROM lisp/builtin/net/windows/pltBind.c

any pltBind(Context *CONTEXT_PTR, word n)
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    initializeWindowsSockets();

    SOCKADDR_IN server;
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(n); // listen on telnet port 23

    // create our socket
    SOCKET sock=socket(AF_INET,SOCK_STREAM,0);

    if(sock == INVALID_SOCKET)
    {
        return Nil;
    }

    // bind our socket to a port(port 123)
    if( bind(sock,(SOCKADDR*)&server,sizeof(server)) !=0 )
    {
        return Nil;
    }

    NewExternalSocket(e, sock);

    any r = cons(CONTEXT_PTR, Nil, Nil);
    car(r) = (any)e;
    setCARType(r, EXT);

    return r;
}
// THIS IS FROM lisp/builtin/net/windows/pltListen.c

any pltListen(Context *CONTEXT_PTR, word n)
{
    int server_fd = (int)n;
    int new_socket, valread;

    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if(listen(server_fd,5) != 0)
    {
        return 0;
    }

    SOCKET client;

    //SOCKADDR_IN from;
    SOCKADDR from;
    int fromlen = sizeof(from);

    // accept connections
    client = accept(server_fd, &from, &fromlen);

    NewExternalSocket(e, client);

    any r = cons(CONTEXT_PTR, Nil, Nil);
    setCARType(r, EXT);
    car(r) = (any)e;

    return r;
}
// THIS IS FROM lisp/builtin/net/windows/getStdinNet.c

void getStdinNet(Context *CONTEXT_PTR)
{
    unsigned char buf[1];

    SOCKET current_client = (SOCKET)CONTEXT_PTR->InFile;

    int res = recv(current_client,buf,1,0);
    CONTEXT_PTR->Chr = buf[0];
}
// THIS IS FROM lisp/builtin/net/windows/pltSocket.c

any pltSocket(Context *CONTEXT_PTR, any ex)
{
    uword n;
    any x,y;
    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
        return Nil;

    external *e = (external*)car(y);
    n = (uword)e->pointer;

    inFrame f;
    outFrame fo;

    f.fp = fo.fp = (FILE*)n;
    f.get = getStdinNet;
    fo.put = putStdoutNet;

    pushIOFilesNet(CONTEXT_PTR, &f, &fo);
    x = prog(CONTEXT_PTR, cddr(ex));
    popIOFilesNet(CONTEXT_PTR);

    e->release(e);
    car(y) = NULL; // TODO -> this has to be understood more
    return Nil;
}
// THIS IS FROM lisp/builtin/net/windows/initializeWindowsSockets.c

static WSADATA wsaData;
void initializeWindowsSockets()
{
    if (wsaData.wVersion)
    {
        // wsaData is already initialized
        return;
    }

    int ret = WSAStartup(0x101, &wsaData); // use highest version of winsock avalible
    if(ret != 0)
    {
        fprintf(stderr, "WSAStartup failed %d\n", ret);
        exit(0);
    }
}

// THIS IS FROM lisp/builtin/net/windows/pltConnect.c

any pltConnect(Context *CONTEXT_PTR, any ex)
{
    initializeWindowsSockets();

    char *targetHostName;
    uword n;
    any x,y;

    x = cdr(ex);
    if (isNil(y = EVAL(CONTEXT_PTR, car(x))))
    {
        targetHostName = (char*)malloc(10);
        strcpy(targetHostName, "127.0.0.1");
    }
    else if (isSym(y))
    {
        int len = symBytes(CONTEXT_PTR, y);
        targetHostName = (char*)malloc(len + 1);
        sym2str(CONTEXT_PTR, y, targetHostName);
    }

    x = cdr(x);
    y = EVAL(CONTEXT_PTR, car(x));

    NeedNum(ex,y);
    n = mp_get_i32(num(y));

    word client_socket;
    int valread;
    struct sockaddr_in serv_addr;

    // Creating socket file descriptor
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(n);
    serv_addr.sin_addr.s_addr = inet_addr(targetHostName);


    free(targetHostName);

    if (connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return Nil;
    }

    NewExternalSocket(e, client_socket);

    any r = cons(CONTEXT_PTR, Nil, Nil);
    setCARType(r, EXT);
    car(r) = e;

    return r;
}
// THIS IS FROM lisp/builtin/net/windows/popIOFilesNet.c

void popIOFilesNet(Context *CONTEXT_PTR)
{

    CONTEXT_PTR->Chr = CONTEXT_PTR->Env.inFrames->next;
    CONTEXT_PTR->Env.get = CONTEXT_PTR->Env.inFrames->get;
    CONTEXT_PTR->InFile = (CONTEXT_PTR->Env.inFrames = CONTEXT_PTR->Env.inFrames->link)?  CONTEXT_PTR->Env.inFrames->fp : stdin;

    CONTEXT_PTR->Env.put = CONTEXT_PTR->Env.outFrames->put;
    CONTEXT_PTR->OutFile = (CONTEXT_PTR->Env.outFrames = CONTEXT_PTR->Env.outFrames->link)? CONTEXT_PTR->Env.outFrames->fp : stdout;
}
// THIS IS FROM lisp/builtin/net/windows/pushIOFilesNet.c

void pushIOFilesNet(Context *CONTEXT_PTR, inFrame *f, outFrame *fo)
{
    f->next = CONTEXT_PTR->Chr,  CONTEXT_PTR->Chr = 0;
    CONTEXT_PTR->InFile = f->fp;
    f->get = CONTEXT_PTR->Env.get,  CONTEXT_PTR->Env.get = getStdinNet;
    f->link = CONTEXT_PTR->Env.inFrames,  CONTEXT_PTR->Env.inFrames = f;

    CONTEXT_PTR->OutFile = fo->fp;
    fo->put = CONTEXT_PTR->Env.put,  CONTEXT_PTR->Env.put = putStdoutNet;
    fo->link = CONTEXT_PTR->Env.outFrames,  CONTEXT_PTR->Env.outFrames = fo;
}
// THIS IS FROM lisp/builtin/thread/windows/thread.c

typedef void * (*thread_func_t)(void *);
void plt_thread_start(Context *CONTEXT_PTR, thread_func_t FUNC, int wait);

typedef struct _PACK
{
    Context *C;
    thread_func_t F;
} PACK;

DWORD WINAPI receive_cmds(LPVOID lpParam)
{
    PACK *P = (PACK*)lpParam;
    P->F(P->C);
    free(P);
    ExitThread(0);
    return 0;
}

void plt_thread_start(Context *CONTEXT_PTR, thread_func_t FUNC, int wait)
{
    DWORD thread;

   // PACK *P = (PACK*)calloc(sizeof(PACK), 1);
   // P->C = CONTEXT_PTR;
   // P->F = FUNC;

    // TODO - wait is not supported yet
    if (wait)
    {
        printf("Waiting for the completion of a thread is not supported yet\n");
    }
    _beginthread(FUNC, 0, CONTEXT_PTR);

    //CreateThread(NULL, 0,receive_cmds,(LPVOID)P, 0, &thread);
}

any pltGetThreadId(Context *CONTEXT_PTR)
{
    mp_int *n = (mp_int*)malloc(sizeof(mp_int));
    mp_err _mp_error = mp_init(n); // TODO handle the errors appropriately
    mp_set(n, GetCurrentThreadId());

    NewNumber(ext, n, r);
    return r;
}

word GetThreadID()
{
    return (word)GetCurrentThreadId();
}
// THIS IS FROM lisp/builtin/thread/windows/sleep.c

int nanosleep(const struct timespec *req, struct timespec *rem);

void plt_sleep(int milliseconds)
{
    Sleep(milliseconds);
}
