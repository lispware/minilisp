#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>

void giveup(char *msg)
{
   fprintf(stderr, "gen: %s\n", msg);
   exit(1);
}

void noReadMacros(void)
{
   giveup("Can't support read-macros");
}

void eofErr(void)
{
   giveup("EOF Overrun");
}

#if INTPTR_MAX == INT32_MAX
    #define WORD_TYPE uint32_t
    #define SIGNED_WORD_TYPE int32_t
    #define WORD_FORMAT_STRING "0x%x"
    #define MEMFILE "mem32.c"
    #define DEFFILE "def32.d"
#elif INTPTR_MAX == INT64_MAX
    #define WORD_TYPE uint64_t
    #define SIGNED_WORD_TYPE int64_t
    #define WORD_FORMAT_STRING "0x%llx"
    #define MEMFILE "mem64.c"
    #define DEFFILE "def64.d"
#else
    #error "Unsupported bit width"
#endif

typedef WORD_TYPE LISP_WORD_SIZE;
typedef SIGNED_WORD_TYPE SIGNED_WORD;

#define BITS (8*((int)sizeof(LISP_WORD_SIZE)))

typedef enum {NO,YES} BOOL;


typedef union
{
    unsigned char parts[4];
    LISP_WORD_SIZE _t;
} PartType;

typedef struct Cell
{
    struct Cell * CAR;
    struct Cell * CDR;
    PartType type;
} Cell, *CellPtr;

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


#define Nil     (0)
#define T       (1)
#define Quote   (2)

static int skip(void);
static BOOL testEsc(void);
static int read0(BOOL top);
static void mkSym(char *, char *, CellPartType);
static void addMem(char *);
void addWord(WORD_TYPE);
LISP_WORD_SIZE mkType(CellPartType carType, CellPartType cdrType);
LISP_WORD_SIZE mkConsType(CellPartType carType, CellPartType cdrType);
static int ramSym(char *name, char *value, CellPartType type);
void addSym(int x);
static int cons(int x, int y);
void addType(CellPartType type);

int Chr, MemIdx;
static char Delim[] = " \t\n\r\"'(),[]`~{}";
static char Token[1024];

typedef struct symbol
{
   char *nm;
   int val;
   struct symbol *less, *more;
} symbol;

static int lookup(symbol **tree, char *name);

static symbol *Intern;
static char **MemGen;

static void insert(symbol **tree, char *name, int value)
{
   symbol *p, **t;

   p = malloc(sizeof(symbol));
   p->nm = strdup(name);
   p->val = value;
   p->less = p->more = NULL;
   for (t = tree;  *t;  t = strcmp(name, (*t)->nm) >= 0? &(*t)->more : &(*t)->less);
   *t = p;
}

void addSym(int x)
{
    char buf[100];
    sprintf(buf, "(any)(Mem + %d)", x);
    addMem(buf);  
}

static int lookup(symbol **tree, char *name)
{
   symbol *p;
   int n;

   for (p = *tree;  p;  p = n > 0? p->more : p->less)
   {
      if ((n = strcmp(name, p->nm)) == 0)
      {
         return p->val;
      }
   }
   return -1;
}

static void mkSym(char *name, char *value, CellPartType type)
{
    int i=0, d;
    WORD_TYPE w=0, c;
    int bin = 0;
    int bin_start=MemIdx;

    while(*name)
    {
        if (!bin && i == BITS)
        {
            // deal with BIN

            addWord(0);
            char buf[100];
            sprintf(buf, "((any)(Mem + %d))", MemIdx+2);
            MemGen[MemIdx-1] = strdup(buf);
            addWord(0);
            addType(mkType(BIN_START, PTR_CELL));

            addWord(w);
            addWord(0);
            addType(mkType(BIN, PTR_CELL));

            bin = 1;
            i = 0;
            w = 0;
        }
        else if (i == BITS)
        {
            char buf[100];
            sprintf(buf, "((any)(Mem + %d))", MemIdx);
            MemGen[MemIdx-2] = strdup(buf);

            addWord(w);
            addWord(0);
            addType(mkType(BIN, PTR_CELL));

            i = 0;
            w = 0;
        }

        c = *name++;
        w |= (c << i);
        i+=8;
    }

    if (bin)
    {
            char buf[100];
            sprintf(buf, "((any)(Mem + %d))", MemIdx);
            MemGen[MemIdx-2] = strdup(buf);

            addWord(w);
            addWord(0);
            addType(mkType(BIN, PTR_CELL));
    }
    else
    {
        addWord(w);
        addWord(0);
        addType(mkType(TXT, type));
    }
}

void addType(CellPartType type)
{
    char buf[100];
    sprintf(buf, WORD_FORMAT_STRING, (LISP_WORD_SIZE)type);
    addMem(buf);
}

LISP_WORD_SIZE mkConsType(CellPartType carType, CellPartType cdrType)
{
    PartType t = {0};
    t.parts[0] = carType;
    t.parts[1] = cdrType;
    return t._t;
}

LISP_WORD_SIZE mkType(CellPartType carType, CellPartType cdrType)
{
    PartType t = {0};
    t.parts[0] = carType;
    t.parts[1] = cdrType;
    return t._t;
}


void addMem(char *v)
{
    MemGen = realloc(MemGen, (MemIdx + 1) * sizeof(char*));
    MemGen[MemIdx++] = strdup(v);
}

void addWord(WORD_TYPE w)
{
    char buf[100];
    sprintf(buf, "(" WORD_FORMAT_STRING ")", w);
    addMem(buf);
}

/* Test for escaped characters */
static BOOL testEsc(void)
{
   for (;;)
   {
      if (Chr < 0)
      {
         return NO;
      }

      if (Chr != '\\')
      {
         return YES;
      }

      if (Chr = getchar(), Chr != '\n')
      {
         return YES;
      }

      do
      {
         Chr = getchar();
      }
      while (Chr == ' '  ||  Chr == '\t');
   }
}

static int skip(void)
{
   for (;;)
   {
      if (Chr < 0)
      {
         return Chr;
      }
      while (Chr <= ' ')
      {
         Chr = getchar();
         if (Chr < 0)
         {
            return Chr;
         }
      }

      if (Chr != '#')
      {
         return Chr;
      }
      Chr = getchar();
      if (Chr != '{')
      {
         while (Chr != '\n')
         {
            if (Chr < 0)
            {
               return Chr;
            }
            Chr = getchar();
         }
      }
      else
      {
         for (;;)
         {
            Chr = getchar();
            if (Chr < 0)
            {
               return Chr;
            }
            if (Chr == '}' && (Chr = getchar(), Chr == '#'))
            {
               break;
            }
         }
         Chr = getchar();
      }
   }
}

/* Read a list */
static int rdList(int z)
{
   int x;

   if (skip() == ')')
   {
      Chr = getchar();
      return Nil;
   }

   if (Chr == ']')
   {
      return Nil;
   }

   if (Chr == '~')
   {
      noReadMacros();
   }

   if (Chr == '.')
   {
      Chr = getchar();
      x = skip()==')' || Chr==']'? z : read0(NO);
      if (skip() == ')')
      {
         Chr = getchar();
      }
      else if (Chr != ']')
      {
         giveup("Bad dotted pair");
      }


      return x;
   }

   x = read0(NO);
   int y = rdList(z ? z : x);
   return cons(x, y);
}

static int ramSym(char *name, char *value, CellPartType type)
{
   int ix = MemIdx;

   mkSym(name, value, type);
   return ix;
}

static int cons(int x, int y)
{
   int i, ix = MemIdx;
   char car[40], cdr[40];

   addSym(x);
   addSym(y);
   addType(mkConsType(PTR_CELL, PTR_CELL));

   return ix;
}

static int read0(BOOL top)
{
    int x;
    WORD_TYPE w;
    char *p, buf[100];

    if (skip() < 0)
    {
        if (top)
            return Nil;
        eofErr();
    }


    if (Chr == '(')
    {
        Chr = getchar();
        x = rdList(0);
        if (top  &&  Chr == ']')
        {
            Chr = getchar();
        }
        return x;
    }

    if (Chr == '[')
    {
        Chr = getchar();
        x = rdList(0);
        if (Chr != ']')
        {
            giveup("Super parentheses mismatch");
        }
        Chr = getchar();
        return x;
    }

    if (Chr == '\'')
    {
        Chr = getchar();
        return cons(Quote, read0(top));
    }

    if (Chr == '`')
    {
        noReadMacros();
    }

    if (Chr == '"')
    {
        Chr = getchar();
        if (Chr == '"')
        {
            Chr = getchar();
            return Nil;
        }

        for (p = Token;;)
        {
            if (!testEsc())
            {
                eofErr();
            }
            *p++ = Chr;
            if (p == Token+1024)
            {
                giveup("Token too long");
            }

            if ((Chr = getchar()) == '"')
            {
                Chr = getchar();
                break;
            }
        }

        *p = '\0';

        if ((x = lookup(&Intern, Token)) >= 0)
        {
            return x;
        }

        sprintf(buf,"(Mem+%d)", MemIdx);
        insert(&Intern, Token, x = ramSym(Token, buf, PTR_CELL));
        return x;
    }

    if (strchr(Delim, Chr))
    {
        giveup("Bad input");
    }

    if (Chr == '\\')
    {
        Chr = getchar();
    }

    for (p = Token;;)
    {
        *p++ = Chr;
        if (p == Token+1024)
        {
            giveup("Token too long");
        }

        Chr = getchar();
        if (strchr(Delim, Chr))
        {
            break;
        }

        if (Chr == '\\')
        {
            Chr = getchar();
        }
    }

    *p = '\0';

    w = strtol(Token, &p, 10);
    if (p != Token && *p == '\0')
    {
        x = MemIdx;
        addWord(w);
        addWord(0);
        addType(mkType(NUM, PTR_CELL));
        return x;
    }

    if ((x = lookup(&Intern, Token)) >= 0)
    {
        return x;
    }

    insert(&Intern, Token, x = ramSym(Token, "0/*Undefined1*/", PTR_CELL));
    return x;
} 

int main(int ac, char *av[])
{
    char buf[100];
    char *p;
    int x; 
    FILE *fpSYM;
    FILE *fpMem = fopen(MEMFILE, "w");

    if ((fpSYM = fopen(DEFFILE, "w")) == NULL)
    {
        giveup("Can't create output files");
    }

    fprintf(fpSYM, "#ifndef __SYM_D__\n");
    fprintf(fpSYM, "#define __SYM_D__\n");

    fprintf(fpSYM, "extern any Mem[];\n");
    fprintf(fpSYM, "#define Nil ((any)(CONTEXT_PTR->Mem+0))\n");
    fprintf(fpSYM, "#define T ((any)(CONTEXT_PTR->Mem+6))\n");

    ac--;

    x = ramSym("Nil", "(Mem)", PTR_CELL);
    x = ramSym("Nil", "(Mem)", PTR_CELL);
    insert(&Intern, "Nil", x);

    x = ramSym("T", "(Mem)", PTR_CELL);
    insert(&Intern, "Nil", x);

    do
    {
        char *n = *++av;
        printf("Loading file %s\n", n);
        if (!freopen(n, "r", stdin))
        {
            giveup("Can't open input file");
        }

        Chr = getchar();
        while ((x = read0(YES)) != Nil)
        {
            if (skip() == '[')
            {                   // C Identifier
                fprintf(fpSYM, "#define ");
                for (;;)
                {
                    Chr = getchar();
                    if (Chr == EOF)
                    {
                        break;
                    }

                    if (Chr == ']')
                    {
                        Chr = getchar();
                        break;
                    }

                    putc(Chr, fpSYM);
                }

                //print(buf, x);
                fprintf(fpSYM, " (any)(CONTEXT_PTR->Mem+%d)\n", x);
                sprintf(buf, "((any)(CONTEXT_PTR->Mem + 0))");
                MemGen[x+1] = strdup(buf);
                //sprintf(buf, "0x%x", mkType(TXT, PTR_CELL));
                //MemGen[x+2] = strdup(buf);
            }

            if (skip() == '{')
            {                   // Function pointer
                fprintf(fpSYM, "#define ");
                for (p = Token;;)
                {
                    Chr = getchar();
                    if (Chr == EOF)
                    {
                        break;
                    }

                    if (Chr == '}')
                    {
                        Chr = getchar();
                        break;
                    }

                    *p++ = Chr;
                    putc(Chr, fpSYM);
                }

                *p = '\0';
                fprintf(fpSYM, "_D (any)(CONTEXT_PTR->Mem+%d)\n", x);
                sprintf(buf, "((any)(%s))", Token);
                MemGen[x+1] = strdup(buf);
                if (!strcmp(MemGen[x+2], "0x407"))
                {
                    sprintf(buf, WORD_FORMAT_STRING, (WORD_TYPE) mkType(BIN_START, FUNC));
                }
                else
                {
                    sprintf(buf, WORD_FORMAT_STRING, (WORD_TYPE) mkType(TXT, FUNC));
                }
                MemGen[x+2] = strdup(buf);
                fprintf(fpSYM, "any %s(Context *, any);\n", Token);
            }
            else
            {                                 // Value
                int v = read0(YES);
                sprintf(buf, "(Mem+%d)", v);
                MemGen[x + 1] = strdup(buf);
                // sprintf(buf, WORD_FORMAT_STRING, mkType(TXT, PTR_CELL));
                // MemGen[x + 2] = strdup(buf);
            }

            while (skip() == ',')          // Properties
            {
                Chr = getchar();
                if (Chr == EOF)
                {
                    break;
                }

                //print(buf, read0(YES));


                //print(buf, (RamIx-4) << 2);
                //Ram[x-1] = strdup(buf);
                //sprintf(buf, "%d", Symbol);
                //Ram[x+1] = strdup(buf);
            }
        }
    }
    while (--ac);

    // x = ramSym("abc", "10", Type_Num);
    // printf("%d\n", x);
    // x = ramSym("abcdefghij", "10", Type_Num);
    // printf("%d\n", x);
    // x = ramSym("abcdefghij", "20", Type_Num);
    // printf("%d\n", x);
    // x = ramSym("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijkl", "20", Type_Num);
    // printf("%d\n", x);

    fprintf(fpMem, "#ifndef __MEM_D__\n");
    fprintf(fpMem, "#define __MEM_D__\n");

    fprintf(fpMem, "#include \"lisp.h\"\n");
    fprintf(fpMem, "#include \"cell.h\"\n");
    fprintf(fpMem, "#include \"%s\"\n", DEFFILE);

    fprintf(fpMem, "any Mem[] = {\n");

    fprintf(fpMem, "    (any)(Mem + 0), (any)(Mem + 0), (any)(0x404),\n");
    for (int i = 3; i < MemIdx; i += 3)
    {
        fprintf(fpMem, "    (any)%s, (any)%s, (any)%s,\n", MemGen[i], MemGen[i + 1], MemGen[i + 2]);
    }
    fprintf(fpMem, "};\n");
    fprintf(fpMem, "#endif\n");
    fclose(fpMem);


    fprintf(fpSYM, "\n#define MEMS %d\n", MemIdx);
    fprintf(fpSYM, "\n#endif\n");
    fclose(fpSYM);

    return 0;
}
