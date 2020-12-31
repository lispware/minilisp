#ifndef __SYM_D__
#define __SYM_D__
#define Nil ((any)(Mem+0))
#define T ((any)(Mem+6))
#define At (any)(Mem+9)
#define At2 (any)(Mem+12)
#define At3 (any)(Mem+15)
#define Zero (any)(Mem+18)
#define doBye_D (any)(Mem+24)
any doBye(any);
#define doDe_D (any)(Mem+27)
any doDe(any);
#define doAdd_D (any)(Mem+30)
any doAdd(any);
#define doSub_D (any)(Mem+33)
any doSub(any);
#define doMul_D (any)(Mem+36)
any doMul(any);
#define doLet_D (any)(Mem+39)
any doLet(any);
#define doPrin_D (any)(Mem+42)
any doPrin(any);
#define doDo_D (any)(Mem+45)
any doDo(any);
#define doSetq_D (any)(Mem+48)
any doSetq(any);
#define doMake_D (any)(Mem+51)
any doMake(any);
#define doLink_D (any)(Mem+54)
any doLink(any);
#define doCons_D (any)(Mem+57)
any doCons(any);
#define doCar_D (any)(Mem+60)
any doCar(any);
#define doCdr_D (any)(Mem+63)
any doCdr(any);
#define doQuote_D (any)(Mem+66)
any doQuote(any);
#define doDump_D (any)(Mem+69)
any doDump(any);
#define doEq_D (any)(Mem+72)
any doEq(any);
#define doIf_D (any)(Mem+75)
any doIf(any);
#endif
