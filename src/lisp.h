#ifndef __LISP_H__
#define __LISP_H__

#include <stdint.h>

#ifndef CELLS
#define CELLS (1024*1024/sizeof(cell))
#endif

#if INTPTR_MAX == INT32_MAX
    #define WORD_TYPE uint32_t
    #define SIGNED_WORD_TYPE int32_t
    #define WORD_FORMAT_STRING "0x%x"
    #define WORD_FORMAT_STRING_D "%d"
#elif INTPTR_MAX == INT64_MAX
    #define WORD_TYPE uint64_t
    #define SIGNED_WORD_TYPE int64_t
    #define WORD_FORMAT_STRING "0x%llx"
    #define WORD_FORMAT_STRING_D "%lld"
#else
    #error "Unsupported bit width"
#endif

#define WORD ((int)sizeof(WORD_TYPE))
#define BITS (8*WORD)

typedef SIGNED_WORD_TYPE word;
typedef WORD_TYPE uword;
typedef unsigned char byte;
typedef unsigned char *ptr;

typedef enum {NO,YES} bool;

typedef union
{
    unsigned char parts[4];
    uword _t;
}
PartType;

// PicoLisp primary data type
typedef struct _cell
{
   struct _cell *car;
   struct _cell *cdr;
   PartType type;
}
cell, *any;

typedef any (*FunPtr)(any);

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

#endif
