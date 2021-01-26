#include "lisp.h"
#include "cell.h"

#if INTPTR_MAX == INT32_MAX
    #include "def32.d"
#elif INTPTR_MAX == INT64_MAX
    #include "def64.d"
#else
    #error "Unsupported bit width"
#endif


any doNot(any x) {
   any a;

   if (isNil(a = EVAL(cadr(x))))
      return T;
   val(At) = a;
   return Nil;
}
