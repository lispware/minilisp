#ifndef __MEM_D__
#define __MEM_D__
#include "lisp.h"
#include "cell.h"
#include "def32.d"
any Mem[] = {
/* Mem +    0 */    (any)(Mem + 0), (any)(Mem + 0), (any)(0x404),
/* Mem +    3 */    (any)(0x6c694e), (any)((any)(Mem+0)), (any)0x401,
/* Mem +    6 */    (any)(0x54), (any)((any)(Mem+0)), (any)0x401,
/* Mem +    9 */    (any)(0x40), (any)(Mem+3), (any)0x401,
/* Mem +   12 */    (any)(0x4040), (any)(Mem+3), (any)0x401,
/* Mem +   15 */    (any)(0x404040), (any)(Mem+3), (any)0x401,
/* Mem +   18 */    (any)(0x6f72655a), (any)(Mem+21), (any)0x401,
/* Mem +   21 */    (any)(0x0), (any)((any)(Mem+0)), (any)0x402,
/* Mem +   24 */    (any)(0x656e4f), (any)(Mem+27), (any)0x401,
/* Mem +   27 */    (any)(0x1), (any)((any)(Mem+0)), (any)0x402,
/* Mem +   30 */    (any)(0x657962), (any)((any)(doBye)), (any)0x301,
/* Mem +   33 */    (any)(0x6564), (any)((any)(doDe)), (any)0x301,
/* Mem +   36 */    (any)(0x2b), (any)((any)(doAdd)), (any)0x301,
/* Mem +   39 */    (any)(0x2d), (any)((any)(doSub)), (any)0x301,
/* Mem +   42 */    (any)(0x2a), (any)((any)(doMul)), (any)0x301,
/* Mem +   45 */    (any)(0x74656c), (any)((any)(doLet)), (any)0x301,
/* Mem +   48 */    (any)((any)(Mem + 51)), (any)((any)(doPrin)), (any)0x307,
/* Mem +   51 */    (any)(0x6e697270), (any)((any)(Mem + 54)), (any)0x406,
/* Mem +   54 */    (any)(0x6c), (any)((any)(Mem+0)), (any)0x406,
/* Mem +   57 */    (any)(0x6f64), (any)((any)(doDo)), (any)0x301,
/* Mem +   60 */    (any)(0x71746573), (any)((any)(doSetq)), (any)0x301,
/* Mem +   63 */    (any)(0x656b616d), (any)((any)(doMake)), (any)0x301,
/* Mem +   66 */    (any)(0x6b6e696c), (any)((any)(doLink)), (any)0x301,
/* Mem +   69 */    (any)(0x736e6f63), (any)((any)(doCons)), (any)0x301,
/* Mem +   72 */    (any)(0x726163), (any)((any)(doCar)), (any)0x301,
/* Mem +   75 */    (any)(0x726463), (any)((any)(doCdr)), (any)0x301,
/* Mem +   78 */    (any)((any)(Mem + 81)), (any)((any)(doQuote)), (any)0x307,
/* Mem +   81 */    (any)(0x746f7571), (any)((any)(Mem + 84)), (any)0x406,
/* Mem +   84 */    (any)(0x65), (any)((any)(Mem+0)), (any)0x406,
/* Mem +   87 */    (any)(0x706d7564), (any)((any)(doDump)), (any)0x301,
/* Mem +   90 */    (any)((any)(Mem + 93)), (any)((any)(doWhile)), (any)0x307,
/* Mem +   93 */    (any)(0x6c696877), (any)((any)(Mem + 96)), (any)0x406,
/* Mem +   96 */    (any)(0x65), (any)((any)(Mem+0)), (any)0x406,
/* Mem +   99 */    (any)(0x6e69), (any)((any)(doIn)), (any)0x301,
/* Mem +  102 */    (any)(0x74756f), (any)((any)(doOut)), (any)0x301,
/* Mem +  105 */    (any)(0x72616863), (any)((any)(doChar)), (any)0x301,
/* Mem +  108 */    (any)(0x656e696c), (any)((any)(doLine)), (any)0x301,
/* Mem +  111 */    (any)(0x746f6e), (any)((any)(doNot)), (any)0x301,
/* Mem +  114 */    (any)(0x726f66), (any)((any)(doFor)), (any)0x301,
/* Mem +  117 */    (any)(0x6e7572), (any)((any)(doRun)), (any)0x301,
/* Mem +  120 */    (any)(0x7368), (any)((any)(doHS)), (any)0x301,
/* Mem +  123 */    (any)(0x3d), (any)((any)(doEq)), (any)0x301,
/* Mem +  126 */    (any)(0x6669), (any)((any)(doIf)), (any)0x301,
/* Mem +  129 */    (any)(0x47), (any)(Mem+159), (any)0x401,
/* Mem +  132 */    (any)(0x58), (any)((any)(Mem+0)), (any)0x401,
/* Mem +  135 */    (any)(any)(Mem + 132), (any)(any)(Mem + 0), (any)0x404,
/* Mem +  138 */    (any)(0x1), (any)((any)(Mem+0)), (any)0x402,
/* Mem +  141 */    (any)(0x2), (any)((any)(Mem+0)), (any)0x402,
/* Mem +  144 */    (any)(any)(Mem + 132), (any)(any)(Mem + 0), (any)0x404,
/* Mem +  147 */    (any)(any)(Mem + 141), (any)(any)(Mem + 144), (any)0x404,
/* Mem +  150 */    (any)(any)(Mem + 138), (any)(any)(Mem + 147), (any)0x404,
/* Mem +  153 */    (any)(any)(Mem + 36), (any)(any)(Mem + 150), (any)0x404,
/* Mem +  156 */    (any)(any)(Mem + 153), (any)(any)(Mem + 0), (any)0x404,
/* Mem +  159 */    (any)(any)(Mem + 135), (any)(any)(Mem + 156), (any)0x404,
/* Mem +  162 */    (any)(0x5a), (any)(Mem+165), (any)0x401,
/* Mem +  165 */    (any)(0xa), (any)((any)(Mem+0)), (any)0x402,
/* Mem +  168 */    (any)((any)(Mem + 171)), (any)(Mem+180), (any)0x407,
/* Mem +  171 */    (any)(0x44434241), (any)((any)(Mem + 174)), (any)0x406,
/* Mem +  174 */    (any)(0x48474645), (any)((any)(Mem + 177)), (any)0x406,
/* Mem +  177 */    (any)(0x4a49), (any)((any)(Mem+0)), (any)0x406,
/* Mem +  180 */    (any)(0xa), (any)((any)(Mem+0)), (any)0x402,
/* Mem +  183 */    (any)((any)(Mem + 186)), (any)((any)(doLongFunc)), (any)0x307,
/* Mem +  186 */    (any)(0x44434241), (any)((any)(Mem + 189)), (any)0x406,
/* Mem +  189 */    (any)(0x48474645), (any)((any)(Mem + 192)), (any)0x406,
/* Mem +  192 */    (any)(0x4b4a49), (any)((any)(Mem+0)), (any)0x406,
/* Mem +  195 */    (any)((any)(Mem + 198)), (any)((any)(doVeryLongFunc)), (any)0x307,
/* Mem +  198 */    (any)(0x44434241), (any)((any)(Mem + 201)), (any)0x406,
/* Mem +  201 */    (any)(0x48474645), (any)((any)(Mem + 204)), (any)0x406,
/* Mem +  204 */    (any)(0x42414a49), (any)((any)(Mem + 207)), (any)0x406,
/* Mem +  207 */    (any)(0x46454443), (any)((any)(Mem + 210)), (any)0x406,
/* Mem +  210 */    (any)(0x4a494847), (any)((any)(Mem + 213)), (any)0x406,
/* Mem +  213 */    (any)(0x4b), (any)((any)(Mem+0)), (any)0x406,
/* Mem +  216 */    (any)(0x6c6c6163), (any)((any)(doCall)), (any)0x301,
/* Mem +  219 */    (any)(0x6b636170), (any)((any)(doPack)), (any)0x301,
/* Mem +  222 */    (any)(0x6b726f66), (any)((any)(doFork)), (any)0x301,
/* Mem +  225 */    (any)((any)(Mem + 228)), (any)((any)(doSleep)), (any)0x307,
/* Mem +  228 */    (any)(0x65656c73), (any)((any)(Mem + 231)), (any)0x406,
/* Mem +  231 */    (any)(0x70), (any)((any)(Mem+0)), (any)0x406,
/* Mem +  234 */    (any)(0x6f69), (any)((any)(doIO)), (any)0x301,
};
#endif
