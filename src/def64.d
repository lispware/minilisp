#ifndef __SYM_D__
#define __SYM_D__
extern any Mem[];
#define Nil ((any)(Mem+0))
#define T ((any)(Mem+6))
#define At (any)(Mem+9)
#define At2 (any)(Mem+12)
#define At3 (any)(Mem+15)
#define Zero (any)(Mem+18)
#define One (any)(Mem+24)
#define doBye_D (any)(Mem+30)
any doBye(any);
#define doDe_D (any)(Mem+33)
any doDe(any);
#define doAdd_D (any)(Mem+36)
any doAdd(any);
#define doSub_D (any)(Mem+39)
any doSub(any);
#define doMul_D (any)(Mem+42)
any doMul(any);
#define doLet_D (any)(Mem+45)
any doLet(any);
#define doPrin_D (any)(Mem+48)
any doPrin(any);
#define doDo_D (any)(Mem+51)
any doDo(any);
#define doSetq_D (any)(Mem+54)
any doSetq(any);
#define doMake_D (any)(Mem+57)
any doMake(any);
#define doLink_D (any)(Mem+60)
any doLink(any);
#define doCons_D (any)(Mem+63)
any doCons(any);
#define doCar_D (any)(Mem+66)
any doCar(any);
#define doCdr_D (any)(Mem+69)
any doCdr(any);
#define doQuote_D (any)(Mem+72)
any doQuote(any);
#define doDump_D (any)(Mem+75)
any doDump(any);
#define doWhile_D (any)(Mem+78)
any doWhile(any);
#define doIn_D (any)(Mem+81)
any doIn(any);
#define doOut_D (any)(Mem+84)
any doOut(any);
#define doChar_D (any)(Mem+87)
any doChar(any);
#define doLine_D (any)(Mem+90)
any doLine(any);
#define doNot_D (any)(Mem+93)
any doNot(any);
#define doFor_D (any)(Mem+96)
any doFor(any);
#define doRun_D (any)(Mem+99)
any doRun(any);
#define doHS_D (any)(Mem+102)
any doHS(any);
#define doEq_D (any)(Mem+105)
any doEq(any);
#define doIf_D (any)(Mem+108)
any doIf(any);
#define doLongFunc_D (any)(Mem+162)
any doLongFunc(any);
#define doVeryLongFunc_D (any)(Mem+171)
any doVeryLongFunc(any);

#define MEMS 183

#endif
