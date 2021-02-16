#ifndef __MEM_D__
#define __MEM_D__
#include "lisp.h"
#include "cell.h"
#include "def64.d"
any Mem[] = {
    (any)(Mem + 0), (any)(Mem + 0), (any)(0x404),
    (any)(0x6c694e), (any)(0x0), (any)0x401,
    (any)(0x54), (any)(0x0), (any)0x401,
    (any)(0x40), (any)(Mem+3), (any)0x401,
    (any)(0x4040), (any)(Mem+3), (any)0x401,
    (any)(0x404040), (any)(Mem+3), (any)0x401,
    (any)(0x6f72655a), (any)(Mem+21), (any)0x401,
    (any)(0x0), (any)(0x0), (any)0x402,
    (any)(0x656e4f), (any)(Mem+27), (any)0x401,
    (any)(0x1), (any)(0x0), (any)0x402,
    (any)(0x657962), (any)((any)(doBye)), (any)0x301,
    (any)(0x6564), (any)((any)(doDe)), (any)0x301,
    (any)(0x2b), (any)((any)(doAdd)), (any)0x301,
    (any)(0x2d), (any)((any)(doSub)), (any)0x301,
    (any)(0x2a), (any)((any)(doMul)), (any)0x301,
    (any)(0x74656c), (any)((any)(doLet)), (any)0x301,
    (any)(0x6c6e697270), (any)((any)(doPrin)), (any)0x301,
    (any)(0x6f64), (any)((any)(doDo)), (any)0x301,
    (any)(0x71746573), (any)((any)(doSetq)), (any)0x301,
    (any)(0x656b616d), (any)((any)(doMake)), (any)0x301,
    (any)(0x6b6e696c), (any)((any)(doLink)), (any)0x301,
    (any)(0x736e6f63), (any)((any)(doCons)), (any)0x301,
    (any)(0x726163), (any)((any)(doCar)), (any)0x301,
    (any)(0x726463), (any)((any)(doCdr)), (any)0x301,
    (any)(0x65746f7571), (any)((any)(doQuote)), (any)0x301,
    (any)(0x706d7564), (any)((any)(doDump)), (any)0x301,
    (any)(0x656c696877), (any)((any)(doWhile)), (any)0x301,
    (any)(0x6e69), (any)((any)(doIn)), (any)0x301,
    (any)(0x74756f), (any)((any)(doOut)), (any)0x301,
    (any)(0x72616863), (any)((any)(doChar)), (any)0x301,
    (any)(0x656e696c), (any)((any)(doLine)), (any)0x301,
    (any)(0x746f6e), (any)((any)(doNot)), (any)0x301,
    (any)(0x726f66), (any)((any)(doFor)), (any)0x301,
    (any)(0x6e7572), (any)((any)(doRun)), (any)0x301,
    (any)(0x7368), (any)((any)(doHS)), (any)0x301,
    (any)(0x3d), (any)((any)(doEq)), (any)0x301,
    (any)(0x6669), (any)((any)(doIf)), (any)0x301,
    (any)(0x47), (any)(Mem+141), (any)0x401,
    (any)(0x58), (any)(0x0), (any)0x401,
    (any)(any)(Mem + 114), (any)(any)(Mem + 0), (any)0x404,
    (any)(0x1), (any)(0x0), (any)0x402,
    (any)(0x2), (any)(0x0), (any)0x402,
    (any)(any)(Mem + 114), (any)(any)(Mem + 0), (any)0x404,
    (any)(any)(Mem + 123), (any)(any)(Mem + 126), (any)0x404,
    (any)(any)(Mem + 120), (any)(any)(Mem + 129), (any)0x404,
    (any)(any)(Mem + 36), (any)(any)(Mem + 132), (any)0x404,
    (any)(any)(Mem + 135), (any)(any)(Mem + 0), (any)0x404,
    (any)(any)(Mem + 117), (any)(any)(Mem + 138), (any)0x404,
    (any)(0x5a), (any)(Mem+147), (any)0x401,
    (any)(0xa), (any)(0x0), (any)0x402,
    (any)((any)(Mem + 153)), (any)(Mem+159), (any)0x407,
    (any)(0x4847464544434241), (any)((any)(Mem + 156)), (any)0x406,
    (any)(0x4a49), (any)(0x0), (any)0x406,
    (any)(0xa), (any)(0x0), (any)0x402,
    (any)((any)(Mem + 165)), (any)((any)(doLongFunc)), (any)0x307,
    (any)(0x4847464544434241), (any)((any)(Mem + 168)), (any)0x406,
    (any)(0x4b4a49), (any)(0x0), (any)0x406,
    (any)((any)(Mem + 174)), (any)((any)(doVeryLongFunc)), (any)0x307,
    (any)(0x4847464544434241), (any)((any)(Mem + 177)), (any)0x406,
    (any)(0x4645444342414a49), (any)((any)(Mem + 180)), (any)0x406,
    (any)(0x4b4a494847), (any)(0x0), (any)0x406,
    (any)(0x6c6c6163), (any)((any)(doCall)), (any)0x301,
    (any)(0x6b636170), (any)((any)(doPack)), (any)0x301,
};
#endif
