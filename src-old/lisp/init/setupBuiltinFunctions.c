#include <lisp.h>

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

void setupBuiltinFunctions(any * Mem)
{
    int MEM_SIZE_GUESS = 300;

    *Mem = (any)calloc(sizeof(cell), MEM_SIZE_GUESS);

    any memCell = *Mem, tempCell;

    car(memCell) = *Mem;
    cdr(memCell) = *Mem;
    //memCell->meta.ptr = (any)(PTR_CELL | PTR_CELL << 8);
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
    
    WORD_TYPE end = (WORD_TYPE)memCell;
    WORD_TYPE start = (WORD_TYPE)*Mem;
    MEMS = (end - start)/sizeof(cell);

    if (MEMS > MEM_SIZE_GUESS)
    {
        fprintf(stderr, "MEM_SIZE_GUESS is %d; It should be atleast %d\n", MEM_SIZE_GUESS, MEMS);
    }
#if 0 // TODO - is this needed
    if (MEMS < MEM_SIZE_GUESS)
    {
        fprintf(stderr, "MEM_SIZE_GUESS is %d; %d is sufficient\n", MEM_SIZE_GUESS, MEMS);
    }
#endif
}
