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
#define doDo_D (any)(Mem+57)
any doDo(any);
#define doSetq_D (any)(Mem+60)
any doSetq(any);
#define doMake_D (any)(Mem+63)
any doMake(any);
#define doLink_D (any)(Mem+66)
any doLink(any);
#define doCons_D (any)(Mem+69)
any doCons(any);
#define doCar_D (any)(Mem+72)
any doCar(any);
#define doCdr_D (any)(Mem+75)
any doCdr(any);
#define doQuote_D (any)(Mem+78)
any doQuote(any);
#define doDump_D (any)(Mem+87)
any doDump(any);
#define doWhile_D (any)(Mem+90)
any doWhile(any);
#define doIn_D (any)(Mem+99)
any doIn(any);
#define doOut_D (any)(Mem+102)
any doOut(any);
#define doChar_D (any)(Mem+105)
any doChar(any);
#define doLine_D (any)(Mem+108)
any doLine(any);
#define doNot_D (any)(Mem+111)
any doNot(any);
#define doFor_D (any)(Mem+114)
any doFor(any);
#define doRun_D (any)(Mem+117)
any doRun(any);
#define doHS_D (any)(Mem+120)
any doHS(any);
#define doEq_D (any)(Mem+123)
any doEq(any);
#define doIf_D (any)(Mem+126)
any doIf(any);
#define doLongFunc_D (any)(Mem+183)
any doLongFunc(any);
#define doVeryLongFunc_D (any)(Mem+195)
any doVeryLongFunc(any);
#define doCall_D (any)(Mem+216)
any doCall(any);
#define doPack_D (any)(Mem+219)
any doPack(any);

#define MEMS 222

#endif
