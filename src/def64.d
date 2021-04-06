#ifndef __SYM_D__
#define __SYM_D__
extern any Mem[];
#define Nil ((any)(CONTEXT_PTR->Mem+0))
#define T ((any)(CONTEXT_PTR->Mem+6))
#define At (any)(CONTEXT_PTR->Mem+9)
#define At2 (any)(CONTEXT_PTR->Mem+12)
#define At3 (any)(CONTEXT_PTR->Mem+15)
#define Zero (any)(CONTEXT_PTR->Mem+18)
#define One (any)(CONTEXT_PTR->Mem+24)
#define doBye_D (any)(CONTEXT_PTR->Mem+30)
any doBye(Context *, any);
#define doDe_D (any)(CONTEXT_PTR->Mem+33)
any doDe(Context *, any);
#define doAdd_D (any)(CONTEXT_PTR->Mem+36)
any doAdd(Context *, any);
#define doSub_D (any)(CONTEXT_PTR->Mem+39)
any doSub(Context *, any);
#define doMul_D (any)(CONTEXT_PTR->Mem+42)
any doMul(Context *, any);
#define doLet_D (any)(CONTEXT_PTR->Mem+45)
any doLet(Context *, any);
#define doPrin_D (any)(CONTEXT_PTR->Mem+48)
any doPrin(Context *, any);
#define doDo_D (any)(CONTEXT_PTR->Mem+51)
any doDo(Context *, any);
#define doSetq_D (any)(CONTEXT_PTR->Mem+54)
any doSetq(Context *, any);
#define doMake_D (any)(CONTEXT_PTR->Mem+57)
any doMake(Context *, any);
#define doLink_D (any)(CONTEXT_PTR->Mem+60)
any doLink(Context *, any);
#define doCons_D (any)(CONTEXT_PTR->Mem+63)
any doCons(Context *, any);
#define doCar_D (any)(CONTEXT_PTR->Mem+66)
any doCar(Context *, any);
#define doCdr_D (any)(CONTEXT_PTR->Mem+69)
any doCdr(Context *, any);
#define doQuote_D (any)(CONTEXT_PTR->Mem+72)
any doQuote(Context *, any);
#define doDump_D (any)(CONTEXT_PTR->Mem+75)
any doDump(Context *, any);
#define doWhile_D (any)(CONTEXT_PTR->Mem+78)
any doWhile(Context *, any);
#define doIn_D (any)(CONTEXT_PTR->Mem+81)
any doIn(Context *, any);
#define doOut_D (any)(CONTEXT_PTR->Mem+84)
any doOut(Context *, any);
#define doChar_D (any)(CONTEXT_PTR->Mem+87)
any doChar(Context *, any);
#define doLine_D (any)(CONTEXT_PTR->Mem+90)
any doLine(Context *, any);
#define doNot_D (any)(CONTEXT_PTR->Mem+93)
any doNot(Context *, any);
#define doFor_D (any)(CONTEXT_PTR->Mem+96)
any doFor(Context *, any);
#define doRun_D (any)(CONTEXT_PTR->Mem+99)
any doRun(Context *, any);
#define doHS_D (any)(CONTEXT_PTR->Mem+102)
any doHS(Context *, any);
#define doEq_D (any)(CONTEXT_PTR->Mem+105)
any doEq(Context *, any);
#define doIf_D (any)(CONTEXT_PTR->Mem+108)
any doIf(Context *, any);
#define doLongFunc_D (any)(CONTEXT_PTR->Mem+162)
any doLongFunc(Context *, any);
#define doVeryLongFunc_D (any)(CONTEXT_PTR->Mem+171)
any doVeryLongFunc(Context *, any);
#define doCall_D (any)(CONTEXT_PTR->Mem+183)
any doCall(Context *, any);
#define doPack_D (any)(CONTEXT_PTR->Mem+186)
any doPack(Context *, any);
#define doFork_D (any)(CONTEXT_PTR->Mem+189)
any doFork(Context *, any);
#define doSleep_D (any)(CONTEXT_PTR->Mem+192)
any doSleep(Context *, any);
#define doIO_D (any)(CONTEXT_PTR->Mem+195)
any doIO(Context *, any);
#define doLoad_D (any)(CONTEXT_PTR->Mem+198)
any doLoad(Context *, any);
#define doEval_D (any)(CONTEXT_PTR->Mem+201)
any doEval(Context *, any);
#define doMapcar_D (any)(CONTEXT_PTR->Mem+204)
any doMapcar(Context *, any);

#define MEMS 207

#endif
