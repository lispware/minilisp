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
/* Mem +   30 */    (any)(0x657962), (any)((any)(Mem + 33)), (any)0x401,
/* Mem +   33 */    (any)((any)(doBye)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +   36 */    (any)(0x6564), (any)((any)(Mem + 39)), (any)0x401,
/* Mem +   39 */    (any)((any)(doDe)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +   42 */    (any)(0x2b), (any)((any)(Mem + 45)), (any)0x401,
/* Mem +   45 */    (any)((any)(doAdd)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +   48 */    (any)(0x2d), (any)((any)(Mem + 51)), (any)0x401,
/* Mem +   51 */    (any)((any)(doSub)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +   54 */    (any)(0x2a), (any)((any)(Mem + 57)), (any)0x401,
/* Mem +   57 */    (any)((any)(doMul)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +   60 */    (any)(0x74656c), (any)((any)(Mem + 63)), (any)0x401,
/* Mem +   63 */    (any)((any)(doLet)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +   66 */    (any)((any)(Mem + 69)), (any)((any)(Mem + 75)), (any)0x407,
/* Mem +   69 */    (any)(0x6e697270), (any)((any)(Mem + 72)), (any)0x406,
/* Mem +   72 */    (any)(0x6c), (any)((any)(Mem+0)), (any)0x406,
/* Mem +   75 */    (any)((any)(doPrin))/*DINGO*/, (any)((any)(Mem+0)), (any)0x403/*H*/,
/* Mem +   78 */    (any)(0x6f64), (any)((any)(Mem + 81)), (any)0x401,
/* Mem +   81 */    (any)((any)(doDo)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +   84 */    (any)(0x71746573), (any)((any)(Mem + 87)), (any)0x401,
/* Mem +   87 */    (any)((any)(doSetq)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +   90 */    (any)(0x656b616d), (any)((any)(Mem + 93)), (any)0x401,
/* Mem +   93 */    (any)((any)(doMake)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +   96 */    (any)(0x6b6e696c), (any)((any)(Mem + 99)), (any)0x401,
/* Mem +   99 */    (any)((any)(doLink)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  102 */    (any)(0x736e6f63), (any)((any)(Mem + 105)), (any)0x401,
/* Mem +  105 */    (any)((any)(doCons)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  108 */    (any)(0x726163), (any)((any)(Mem + 111)), (any)0x401,
/* Mem +  111 */    (any)((any)(doCar)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  114 */    (any)(0x726463), (any)((any)(Mem + 117)), (any)0x401,
/* Mem +  117 */    (any)((any)(doCdr)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  120 */    (any)((any)(Mem + 123)), (any)((any)(Mem + 129)), (any)0x407,
/* Mem +  123 */    (any)(0x746f7571), (any)((any)(Mem + 126)), (any)0x406,
/* Mem +  126 */    (any)(0x65), (any)((any)(Mem+0)), (any)0x406,
/* Mem +  129 */    (any)((any)(doQuote))/*DINGO*/, (any)((any)(Mem+0)), (any)0x403/*H*/,
/* Mem +  132 */    (any)(0x706d7564), (any)((any)(Mem + 135)), (any)0x401,
/* Mem +  135 */    (any)((any)(doDump)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  138 */    (any)((any)(Mem + 141)), (any)((any)(Mem + 147)), (any)0x407,
/* Mem +  141 */    (any)(0x6c696877), (any)((any)(Mem + 144)), (any)0x406,
/* Mem +  144 */    (any)(0x65), (any)((any)(Mem+0)), (any)0x406,
/* Mem +  147 */    (any)((any)(doWhile))/*DINGO*/, (any)((any)(Mem+0)), (any)0x403/*H*/,
/* Mem +  150 */    (any)(0x6e69), (any)((any)(Mem + 153)), (any)0x401,
/* Mem +  153 */    (any)((any)(doIn)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  156 */    (any)(0x74756f), (any)((any)(Mem + 159)), (any)0x401,
/* Mem +  159 */    (any)((any)(doOut)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  162 */    (any)(0x72616863), (any)((any)(Mem + 165)), (any)0x401,
/* Mem +  165 */    (any)((any)(doChar)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  168 */    (any)(0x656e696c), (any)((any)(Mem + 171)), (any)0x401,
/* Mem +  171 */    (any)((any)(doLine)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  174 */    (any)(0x746f6e), (any)((any)(Mem + 177)), (any)0x401,
/* Mem +  177 */    (any)((any)(doNot)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  180 */    (any)(0x726f66), (any)((any)(Mem + 183)), (any)0x401,
/* Mem +  183 */    (any)((any)(doFor)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  186 */    (any)(0x6e7572), (any)((any)(Mem + 189)), (any)0x401,
/* Mem +  189 */    (any)((any)(doRun)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  192 */    (any)(0x7368), (any)((any)(Mem + 195)), (any)0x401,
/* Mem +  195 */    (any)((any)(doHS)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  198 */    (any)(0x3d), (any)((any)(Mem + 201)), (any)0x401,
/* Mem +  201 */    (any)((any)(doEq)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  204 */    (any)(0x6669), (any)((any)(Mem + 207)), (any)0x401,
/* Mem +  207 */    (any)((any)(doIf)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  210 */    (any)(0x47), (any)(Mem+240), (any)0x401,
/* Mem +  213 */    (any)(0x58), (any)((any)(Mem+0)), (any)0x401,
/* Mem +  216 */    (any)(any)(Mem + 213), (any)(any)(Mem + 0), (any)0x404,
/* Mem +  219 */    (any)(0x1), (any)((any)(Mem+0)), (any)0x402,
/* Mem +  222 */    (any)(0x2), (any)((any)(Mem+0)), (any)0x402,
/* Mem +  225 */    (any)(any)(Mem + 213), (any)(any)(Mem + 0), (any)0x404,
/* Mem +  228 */    (any)(any)(Mem + 222), (any)(any)(Mem + 225), (any)0x404,
/* Mem +  231 */    (any)(any)(Mem + 219), (any)(any)(Mem + 228), (any)0x404,
/* Mem +  234 */    (any)(any)(Mem + 42), (any)(any)(Mem + 231), (any)0x404,
/* Mem +  237 */    (any)(any)(Mem + 234), (any)(any)(Mem + 0), (any)0x404,
/* Mem +  240 */    (any)(any)(Mem + 216), (any)(any)(Mem + 237), (any)0x404,
/* Mem +  243 */    (any)(0x5a), (any)(Mem+246), (any)0x401,
/* Mem +  246 */    (any)(0xa), (any)((any)(Mem+0)), (any)0x402,
/* Mem +  249 */    (any)((any)(Mem + 252)), (any)(Mem+261), (any)0x407,
/* Mem +  252 */    (any)(0x44434241), (any)((any)(Mem + 255)), (any)0x406,
/* Mem +  255 */    (any)(0x48474645), (any)((any)(Mem + 258)), (any)0x406,
/* Mem +  258 */    (any)(0x4a49), (any)((any)(Mem+0)), (any)0x406,
/* Mem +  261 */    (any)(0xa), (any)((any)(Mem+0)), (any)0x402,
/* Mem +  264 */    (any)((any)(Mem + 267)), (any)((any)(Mem + 276)), (any)0x407,
/* Mem +  267 */    (any)(0x44434241), (any)((any)(Mem + 270)), (any)0x406,
/* Mem +  270 */    (any)(0x48474645), (any)((any)(Mem + 273)), (any)0x406,
/* Mem +  273 */    (any)(0x4b4a49), (any)((any)(Mem+0)), (any)0x406,
/* Mem +  276 */    (any)((any)(doLongFunc))/*DINGO*/, (any)((any)(Mem+0)), (any)0x403/*H*/,
/* Mem +  279 */    (any)((any)(Mem + 282)), (any)((any)(Mem + 300)), (any)0x407,
/* Mem +  282 */    (any)(0x44434241), (any)((any)(Mem + 285)), (any)0x406,
/* Mem +  285 */    (any)(0x48474645), (any)((any)(Mem + 288)), (any)0x406,
/* Mem +  288 */    (any)(0x42414a49), (any)((any)(Mem + 291)), (any)0x406,
/* Mem +  291 */    (any)(0x46454443), (any)((any)(Mem + 294)), (any)0x406,
/* Mem +  294 */    (any)(0x4a494847), (any)((any)(Mem + 297)), (any)0x406,
/* Mem +  297 */    (any)(0x4b), (any)((any)(Mem+0)), (any)0x406,
/* Mem +  300 */    (any)((any)(doVeryLongFunc))/*DINGO*/, (any)((any)(Mem+0)), (any)0x403/*H*/,
/* Mem +  303 */    (any)(0x6c6c6163), (any)((any)(Mem + 306)), (any)0x401,
/* Mem +  306 */    (any)((any)(doCall)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  309 */    (any)(0x6b636170), (any)((any)(Mem + 312)), (any)0x401,
/* Mem +  312 */    (any)((any)(doPack)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  315 */    (any)(0x6b726f66), (any)((any)(Mem + 318)), (any)0x401,
/* Mem +  318 */    (any)((any)(doFork)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  321 */    (any)((any)(Mem + 324)), (any)((any)(Mem + 330)), (any)0x407,
/* Mem +  324 */    (any)(0x65656c73), (any)((any)(Mem + 327)), (any)0x406,
/* Mem +  327 */    (any)(0x70), (any)((any)(Mem+0)), (any)0x406,
/* Mem +  330 */    (any)((any)(doSleep))/*DINGO*/, (any)((any)(Mem+0)), (any)0x403/*H*/,
/* Mem +  333 */    (any)(0x6f69), (any)((any)(Mem + 336)), (any)0x401,
/* Mem +  336 */    (any)((any)(doIO)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  339 */    (any)(0x64616f6c), (any)((any)(Mem + 342)), (any)0x401,
/* Mem +  342 */    (any)((any)(doLoad)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  345 */    (any)(0x6c617665), (any)((any)(Mem + 348)), (any)0x401,
/* Mem +  348 */    (any)((any)(doEval)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  351 */    (any)((any)(Mem + 354)), (any)((any)(Mem + 360)), (any)0x407,
/* Mem +  354 */    (any)(0x6370616d), (any)((any)(Mem + 357)), (any)0x406,
/* Mem +  357 */    (any)(0x7261), (any)((any)(Mem+0)), (any)0x406,
/* Mem +  360 */    (any)((any)(doMapcar))/*DINGO*/, (any)((any)(Mem+0)), (any)0x403/*H*/,
/* Mem +  363 */    (any)((any)(Mem + 366)), (any)((any)(Mem + 375)), (any)0x407,
/* Mem +  366 */    (any)(0x706d6173), (any)((any)(Mem + 369)), (any)0x406,
/* Mem +  369 */    (any)(0x704f656c), (any)((any)(Mem + 372)), (any)0x406,
/* Mem +  372 */    (any)(0x6e65), (any)((any)(Mem+0)), (any)0x406,
/* Mem +  375 */    (any)((any)(doSampleOpen))/*DINGO*/, (any)((any)(Mem+0)), (any)0x403/*H*/,
/* Mem +  378 */    (any)((any)(Mem + 381)), (any)((any)(Mem + 390)), (any)0x407,
/* Mem +  381 */    (any)(0x706d6173), (any)((any)(Mem + 384)), (any)0x406,
/* Mem +  384 */    (any)(0x6552656c), (any)((any)(Mem + 387)), (any)0x406,
/* Mem +  387 */    (any)(0x6461), (any)((any)(Mem+0)), (any)0x406,
/* Mem +  390 */    (any)((any)(doSampleRead))/*DINGO*/, (any)((any)(Mem+0)), (any)0x403/*H*/,
/* Mem +  393 */    (any)(0x646e6962), (any)((any)(Mem + 396)), (any)0x401,
/* Mem +  396 */    (any)((any)(doBind)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  399 */    (any)((any)(Mem + 402)), (any)((any)(Mem + 408)), (any)0x407,
/* Mem +  402 */    (any)(0x7473696c), (any)((any)(Mem + 405)), (any)0x406,
/* Mem +  405 */    (any)(0x6e65), (any)((any)(Mem+0)), (any)0x406,
/* Mem +  408 */    (any)((any)(doListen))/*DINGO*/, (any)((any)(Mem+0)), (any)0x403/*H*/,
/* Mem +  411 */    (any)(0x746b73), (any)((any)(Mem + 414)), (any)0x401,
/* Mem +  414 */    (any)((any)(doSocket)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  417 */    (any)((any)(Mem + 420)), (any)((any)(Mem + 426)), (any)0x407,
/* Mem +  420 */    (any)(0x6e6e6f63), (any)((any)(Mem + 423)), (any)0x406,
/* Mem +  423 */    (any)(0x746365), (any)((any)(Mem+0)), (any)0x406,
/* Mem +  426 */    (any)((any)(doConnect))/*DINGO*/, (any)((any)(Mem+0)), (any)0x403/*H*/,
/* Mem +  429 */    (any)(0x70747468), (any)((any)(Mem + 432)), (any)0x401,
/* Mem +  432 */    (any)((any)(doHTTP)), (any)((any)(Mem+0)), (any)0x403,
/* Mem +  435 */    (any)(0x706f6f6c), (any)((any)(Mem + 438)), (any)0x401,
/* Mem +  438 */    (any)((any)(doLoop)), (any)((any)(Mem+0)), (any)0x403,
};
#endif
