# Overview

minilisp is a port of [PicoLisp](https://software-lab.de/doc/ref.html) with goal of being only dependent on a C compiler and not on POSIX. This makes it more portable. At the lowest level, minilisp is constructed out of a _CELL_ data structure. Unlike PicoLisp, minilisp _CELL_ has an extra _part_ besides CAR and CDR. The extra _part_ is used to hold the types of CAR and CDR instead of relying on pointer tagging. A description of how minilisp is built and run is perhaps the quickest way to get an understanding of how minilisp works.
```C

typedef union
{
    unsigned char parts[4];
    uword _t;
}
PartType;

typedef struct _cell
{
   struct _cell *car;
   struct _cell *cdr;
   union
   {
       PartType type;
       struct _cell *ptr;
   } meta;
}
cell, *any;
```
## How's minilisp built
The starting point of building minilisp is [mem.s](https://github.com/lispware/minilisp/blob/main/src/mem.s). The contents of the file is a list of key-value pairs. Here are a few entries from the file.

Entry|Description
---|---
```prinl {doPrin}``` | Map the symbol _prinl_ to the C function _doPrin_
```Zero [Zero] 0``` | Map the symbol _Zero_ to the C Macro _Zero_ that points to the value 0 
```G ((X) (+ 1 2 X))``` | Map the symbol _G_ to a function that takes in one parameter _X_ and applies it to (+ 1 2 X)

[gen.c](https://github.com/lispware/minilisp/blob/main/src/gen.c) contains the logic to tranlate the key-value pairs in  [mem.s](https://github.com/lispware/minilisp/blob/main/src/mem.s) into one or more _CELLs_ in [mem64.c](https://github.com/lispware/minilisp/blob/main/src/mem64.c) and related C declarations in [def64.d](https://github.com/lispware/minilisp/blob/main/src/def64.d). Note how the translation is simple and literal.

### **prinl {doPrin}** is translated to the following CELL

```C
/* Mem +   48 */    (any)(0x6c6e697270), (any)((any)(doPrin)), (any)0x301,
```
The CAR part of the _CELL_ is set to 0x6c6e697270, which is essentially the ASCII codes for the characters of _prinl_
```
6c -> 108 -> l
6e -> 110 -> n
69 -> 105 -> i
72 -> 114 -> r
70 -> 112 -> p
```
The CDR part of the _CELL_ is set to the C function _doPrin_.
The third part contains 0x301 tells the type of CAR is _TXT_ and the type of CDR is _FUNC_. The type values are defined in _CellPartType_ in [lisp.h](https://github.com/lispware/minilisp/blob/main/src/lisp.h) as follows 
```C
typedef enum
{
    UNDEFINED,
    TXT,
    NUM,
    FUNC,
    PTR_CELL,
    INTERN,
    BIN,
    BIN_START,
} CellPartType;
```

### **Zero [Zero] 0** is translated to the following two CELLs
```C
/* Mem +   18 */    (any)(0x6f72655a), (any)(Mem+21), (any)0x401,
/* Mem +   21 */    (any)(0x0), (any)(0x0), (any)0x402,
```
The CAR part of the first _CELL_ is set to 0x6f72655a, which is essentially the ASCII codes for the chanracters of _Zero_
The CDR part of the _CELL_ is set to (any)(Mem+21), which is a pointer to the second _CELL_. The CAR part of the second _CELL_ contains 0 and the CDR part of the second cell contains 0 \[TODO this should really be a pointer to Nil\].

### **G ((X) (+ 1 2 X))** is translated to the following CELLs

```C
/* Mem +  111 */    (any)(0x47), (any)(Mem+141), (any)0x401,
/* Mem +  114 */    (any)(0x58), (any)(0x0), (any)0x401,
/* Mem +  117 */    (any)(any)(Mem + 114), (any)(any)(Mem + 0), (any)0x404,
/* Mem +  120 */    (any)(0x1), (any)(0x0), (any)0x402,
/* Mem +  123 */    (any)(0x2), (any)(0x0), (any)0x402,
/* Mem +  126 */    (any)(any)(Mem + 114), (any)(any)(Mem + 0), (any)0x404,
/* Mem +  129 */    (any)(any)(Mem + 123), (any)(any)(Mem + 126), (any)0x404,
/* Mem +  132 */    (any)(any)(Mem + 120), (any)(any)(Mem + 129), (any)0x404,
/* Mem +  135 */    (any)(any)(Mem + 36), (any)(any)(Mem + 132), (any)0x404,
/* Mem +  138 */    (any)(any)(Mem + 135), (any)(any)(Mem + 0), (any)0x404,
/* Mem +  141 */    (any)(any)(Mem + 117), (any)(any)(Mem + 138), (any)0x404,
/* Mem +  144 */    (any)(0x5a), (any)(Mem+147), (any)0x401,
```

