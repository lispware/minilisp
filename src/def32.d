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
#define doDe_D (any)(CONTEXT_PTR->Mem+36)
any doDe(Context *, any);
#define doAdd_D (any)(CONTEXT_PTR->Mem+42)
any doAdd(Context *, any);
#define doSub_D (any)(CONTEXT_PTR->Mem+48)
any doSub(Context *, any);
#define doMul_D (any)(CONTEXT_PTR->Mem+54)
any doMul(Context *, any);
#define doLet_D (any)(CONTEXT_PTR->Mem+60)
any doLet(Context *, any);
#define doPrin_D (any)(CONTEXT_PTR->Mem+66)
any doPrin(Context *, any);
#define doDo_D (any)(CONTEXT_PTR->Mem+78)
any doDo(Context *, any);
#define doSetq_D (any)(CONTEXT_PTR->Mem+84)
any doSetq(Context *, any);
#define doMake_D (any)(CONTEXT_PTR->Mem+90)
any doMake(Context *, any);
#define doLink_D (any)(CONTEXT_PTR->Mem+96)
any doLink(Context *, any);
#define doCons_D (any)(CONTEXT_PTR->Mem+102)
any doCons(Context *, any);
#define doCar_D (any)(CONTEXT_PTR->Mem+108)
any doCar(Context *, any);
#define doCdr_D (any)(CONTEXT_PTR->Mem+114)
any doCdr(Context *, any);
#define doQuote_D (any)(CONTEXT_PTR->Mem+120)
any doQuote(Context *, any);
#define doDump_D (any)(CONTEXT_PTR->Mem+132)
any doDump(Context *, any);
#define doWhile_D (any)(CONTEXT_PTR->Mem+138)
any doWhile(Context *, any);
#define doIn_D (any)(CONTEXT_PTR->Mem+150)
any doIn(Context *, any);
#define doOut_D (any)(CONTEXT_PTR->Mem+156)
any doOut(Context *, any);
#define doChar_D (any)(CONTEXT_PTR->Mem+162)
any doChar(Context *, any);
#define doLine_D (any)(CONTEXT_PTR->Mem+168)
any doLine(Context *, any);
#define doNot_D (any)(CONTEXT_PTR->Mem+174)
any doNot(Context *, any);
#define doFor_D (any)(CONTEXT_PTR->Mem+180)
any doFor(Context *, any);
#define doRun_D (any)(CONTEXT_PTR->Mem+186)
any doRun(Context *, any);
#define doHS_D (any)(CONTEXT_PTR->Mem+192)
any doHS(Context *, any);
#define doEq_D (any)(CONTEXT_PTR->Mem+198)
any doEq(Context *, any);
#define doIf_D (any)(CONTEXT_PTR->Mem+204)
any doIf(Context *, any);
#define doLongFunc_D (any)(CONTEXT_PTR->Mem+264)
any doLongFunc(Context *, any);
#define doVeryLongFunc_D (any)(CONTEXT_PTR->Mem+279)
any doVeryLongFunc(Context *, any);
#define doCall_D (any)(CONTEXT_PTR->Mem+303)
any doCall(Context *, any);
#define doPack_D (any)(CONTEXT_PTR->Mem+309)
any doPack(Context *, any);
#define doFork_D (any)(CONTEXT_PTR->Mem+315)
any doFork(Context *, any);
#define doSleep_D (any)(CONTEXT_PTR->Mem+321)
any doSleep(Context *, any);
#define doIO_D (any)(CONTEXT_PTR->Mem+333)
any doIO(Context *, any);
#define doLoad_D (any)(CONTEXT_PTR->Mem+339)
any doLoad(Context *, any);
#define doEval_D (any)(CONTEXT_PTR->Mem+345)
any doEval(Context *, any);
#define doMapcar_D (any)(CONTEXT_PTR->Mem+351)
any doMapcar(Context *, any);
#define doSampleOpen_D (any)(CONTEXT_PTR->Mem+363)
any doSampleOpen(Context *, any);
#define doSampleRead_D (any)(CONTEXT_PTR->Mem+378)
any doSampleRead(Context *, any);
#define doBind_D (any)(CONTEXT_PTR->Mem+393)
any doBind(Context *, any);
#define doListen_D (any)(CONTEXT_PTR->Mem+399)
any doListen(Context *, any);
#define doSocket_D (any)(CONTEXT_PTR->Mem+411)
any doSocket(Context *, any);
#define doConnect_D (any)(CONTEXT_PTR->Mem+417)
any doConnect(Context *, any);
#define doHTTP_D (any)(CONTEXT_PTR->Mem+429)
any doHTTP(Context *, any);
#define doSocketClose_D (any)(CONTEXT_PTR->Mem+435)
any doSocketClose(Context *, any);
#define doLoop_D (any)(CONTEXT_PTR->Mem+447)
any doLoop(Context *, any);
#define doChop_D (any)(CONTEXT_PTR->Mem+453)
any doChop(Context *, any);
#define doGC_D (any)(CONTEXT_PTR->Mem+459)
any doGC(Context *, any);

#define MEMS 465

#endif
