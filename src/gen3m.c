/* 29nov17abu
 * (c) Software Lab. Alexander Burger
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN64
typedef unsigned long long word;
#define LX_FMT "0x%llx"
#else
typedef unsigned long word;
#define LX_FMT "0x%lx"
#endif

typedef unsigned char byte;

#undef bool
typedef enum {NO,YES} bool;

#define txt(n)    (n << 1| 1)
#define box(n)    (n << 3| 6)

#define Nil    (1 << 2)
#define T      (5 << 2)
#define Quote  (7 << 2)

static int Bits, Chr, RomIx, RamIx;
static char **Rom, **Ram;
static char Token[1024];

static int read0(bool);
static char Delim[] = " \t\n\r\"'(),[]`~{}";

typedef struct symbol {
   char *nm;
   int val;
   struct symbol *less, *more;
} symbol;

#define MAXFUNCTIONS 1024
char *functions[MAXFUNCTIONS];
int FUNC_CTR=0;

typedef struct _nextNumber
{
	char address[40];
	word value;
	struct _nextNumber *next;
} NumberList;
NumberList *NL;

static symbol *Intern, *Transient;

static byte Ascii6[] = {
   0,  2,  2,  2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
   2,  2,  2,  2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
   2,  1,  3,  5,  7,  9, 11,  13,  15,  17,  19,  21,  23,  25,   4,   6,
  27, 29, 31, 33, 35, 37, 39,  41,  43,  45,  47,  49,   8,  51,  10,  53,
  55, 57, 59, 61, 63, 65, 67,  69,  71,  73,  75,  77,  79,  81,  83,  85,
  87, 89, 91, 93, 95, 97, 99, 101, 103, 105, 107, 109, 111, 113, 115, 117,
 119, 12, 14, 16, 18, 20, 22,  24,  26,  28,  30,  32,  34,  36,  38,  40,
  42, 44, 46, 48, 50, 52, 54,  56,  58,  60,  62, 121, 123, 125, 127,   0
};

static void giveup(char *msg) {
   fprintf(stderr, "gen3m: %s\n", msg);
   exit(1);
}

static void noReadMacros(void) {
   giveup("Can't support read-macros");
}

static void eofErr(void) {
   giveup("EOF Overrun");
}

#ifdef WIN64
static void addList(int *ix, char ***list, char *fmt, word x) {
#else
static void addList(int *ix, char ***list, char *fmt, word x) {
#endif
   char buf[40];

   *list = realloc(*list, (*ix + 1) * sizeof(char*));
   if (x)
      sprintf(buf, fmt, x);
   (*list)[(*ix)++] = strdup(x? buf : fmt);
}

static void mkSym(int *ix, char ***list, char *mem, char *name, char *value) {
   bool bin;
   int i, c, d;
   word w;

   bin = NO;
   i = (w = Ascii6[*name++ & 127]) & 1? 7 : 6;
   while (*name) {
      d = (c = Ascii6[*name++ & 127]) & 1? 7 : 6;

      if (i != Bits)
         w |= (word)c << i;
      if (i + d  > Bits) {
         if (bin)
            addList(&RomIx, &Rom, "(Rom+%d)", RomIx + 2);
         else {
            addList(ix, list, "(Rom+%d)", RomIx + (ix == &RomIx? 3 : 1));
            addList(ix, list, value, 0);
            bin = YES;
         }
         addList(&RomIx, &Rom, LX_FMT, w);
         w = c >> Bits - i;
         i -= Bits;
      }
      i += d;
   }
   if (bin) {
      if (i <= (Bits-2))
         addList(&RomIx, &Rom, LX_FMT, box(w));
      else {
         addList(&RomIx, &Rom, "(Rom+%d)", RomIx + 2);
         addList(&RomIx, &Rom, LX_FMT, w);
         addList(&RomIx, &Rom, "2", 0);
      }
   }
   else if (i > Bits-1) {
      addList(ix, list, "(Rom+%d)", RomIx + (ix == &RomIx? 3 : 1));
      addList(ix, list, value, 0);
      addList(&RomIx, &Rom, LX_FMT, w);
      addList(&RomIx, &Rom, "2", 0);
   }
   else {
      addList(ix, list, LX_FMT, txt(w));
      addList(ix, list, value, 0);
   }
}

static void addNumber(int ram, int carOrCdr, int idx, int x)
{
	if (!(x&2)) return;
   	   NumberList * oldNL = NL;
   	   NL = (NumberList * )calloc(sizeof(NumberList), 1);
   	   NL->next = oldNL;
   	   char *ram0 = "(Ram+%d)";
   	   char *ram1 = "(Ram+%d+1)";
   	   char *rom0 = "(Rom+%d)";
   	   char *rom1 = "(Rom+%d+1)";
   	   if (ram)
	   {
	   	if (carOrCdr == 0) sprintf(NL->address, ram0, idx);
	   	else sprintf(NL->address, ram1, idx);
	   }
	   else
	   {
	   	if (carOrCdr == 0) sprintf(NL->address, rom0, idx);
	   	else sprintf(NL->address, rom1, idx);
	   }
	   NL->value = x;
}

static void print(char buf[], int x, int where) {
   if (x & 2)
      sprintf(buf, "%d/*_N%d*/", x, where);
   else if ((x >>= 2) > 0)
      sprintf(buf, "(Rom+%d)", x);
   else
      sprintf(buf, "(Ram+%d)", -x);
}

static int cons(int x, int y) {
   int i, ix = RomIx;
   char car[40], cdr[40];

   print(car, x, 0);
   print(cdr, y, 1);
   for (i = 0; i < RomIx;  i += 2)
      if (strcmp(car, Rom[i]) == 0  &&  strcmp(cdr, Rom[i+1]) == 0)
         return i << 2;

   addNumber(0, 0, RomIx, x);
   addNumber(0, 1, RomIx, y);
   addList(&RomIx, &Rom, car, 0);
   addList(&RomIx, &Rom, cdr, 0);
   return ix << 2;
}

static int romSym(char *name, char *value) {
   int ix = RomIx;

   mkSym(&RomIx, &Rom, "(Rom+%d)", name, value);
   return ix + 1 << 2;
}

static int ramSym(char *name, char *value) {
   int ix = RamIx;

   mkSym(&RamIx, &Ram, "(Ram+%d)", name, value);
   return -(ix + 1) << 2;
}

static void insert(symbol **tree, char *name, int value) {
   symbol *p, **t;

   p = malloc(sizeof(symbol));
   p->nm = strdup(name);
   p->val = value;
   p->less = p->more = NULL;
   for (t = tree;  *t;  t = strcmp(name, (*t)->nm) >= 0? &(*t)->more : &(*t)->less);
   *t = p;
}

static int lookup(symbol **tree, char *name) {
   symbol *p;
   int n;

   for (p = *tree;  p;  p = n > 0? p->more : p->less)
      if ((n = strcmp(name, p->nm)) == 0)
         return p->val;
   return 0;
}

static int skip(void) {
   for (;;) {
      if (Chr < 0)
         return Chr;
      while (Chr <= ' ') {
         Chr = getchar();
         if (Chr < 0)
            return Chr;
      }
      if (Chr != '#')
         return Chr;
      Chr = getchar();
      if (Chr != '{') {
         while (Chr != '\n') {
            if (Chr < 0)
               return Chr;
            Chr = getchar();
         }
      }
      else {
         for (;;) {
            Chr = getchar();
            if (Chr < 0)
               return Chr;
            if (Chr == '}' && (Chr = getchar(), Chr == '#'))
               break;
         }
         Chr = getchar();
      }
   }
}

/* Test for escaped characters */
static bool testEsc(void) {
   for (;;) {
      if (Chr < 0)
         return NO;
      if (Chr != '\\')
         return YES;
      if (Chr = getchar(), Chr != '\n')
         return YES;
      do
         Chr = getchar();
      while (Chr == ' '  ||  Chr == '\t');
   }
}

/* Read a list */
static int rdList(int z) {
   int x;

   if (skip() == ')') {
      Chr = getchar();
      return Nil;
   }
   if (Chr == ']')
      return Nil;
   if (Chr == '~')
      noReadMacros();
   if (Chr == '.') {
      Chr = getchar();
      x = skip()==')' || Chr==']'? z : read0(NO);
      if (skip() == ')')
         Chr = getchar();
      else if (Chr != ']')
         giveup("Bad dotted pair");
      return x;
   }
   x = read0(NO);
   return cons(x, rdList(z ? z : x));
}

/* Read one expression */
static int read0(bool top) {
   int x;
   word w;
   char *p, buf[40];

   if (skip() < 0) {
      if (top)
         return Nil;
      eofErr();
   }
   if (Chr == '(') {
      Chr = getchar();
      x = rdList(0);
      if (top  &&  Chr == ']')
         Chr = getchar();
      return x;
   }
   if (Chr == '[') {
      Chr = getchar();
      x = rdList(0);
      if (Chr != ']')
         giveup("Super parentheses mismatch");
      Chr = getchar();
      return x;
   }
   if (Chr == '\'') {
      Chr = getchar();
      return cons(Quote, read0(top));
   }
   if (Chr == '`')
      noReadMacros();
   if (Chr == '"') {
      Chr = getchar();
      if (Chr == '"') {
         Chr = getchar();
         return Nil;
      }
      for (p = Token;;) {
         if (!testEsc())
            eofErr();
         *p++ = Chr;
         if (p == Token+1024)
            giveup("Token too long");
         if ((Chr = getchar()) == '"') {
            Chr = getchar();
            break;
         }
      }
      *p = '\0';
      if (x = lookup(&Transient, Token))
         return x;
      print(buf, -(RamIx + 1) << 2, 2);
      insert(&Transient, Token, x = ramSym(Token, buf));
      return x;
   }
   if (strchr(Delim, Chr))
      giveup("Bad input");
   if (Chr == '\\')
      Chr = getchar();
   for (p = Token;;) {
      *p++ = Chr;
      if (p == Token+1024)
         giveup("Token too long");
      Chr = getchar();
      if (strchr(Delim, Chr))
         break;
      if (Chr == '\\')
         Chr = getchar();
   }
   *p = '\0';
   w = strtol(Token, &p, 10);
   if (p != Token && *p == '\0')
      return box(w);
   if (x = lookup(&Intern, Token))
      return x;
   insert(&Intern, Token, x = ramSym(Token, "(Rom+1)"));
   return x;
}

int main(int ac, char *av[]) {
   int x, ix;
   FILE *fp;
   char *p, buf[1024];

   if ((ac -= 2) <= 0)
      giveup("No input files");
   if ((Bits = atoi(*++av)) == 0)
      Bits = (int)sizeof(char*) * 8;
   if ((fp = fopen("sym.d", "w")) == NULL)
      giveup("Can't create output files");
   insert(&Intern, "NIL", romSym("NIL", "(Rom+1)"));
   cons(Nil, Nil);
   fprintf(fp, "#define Nil (any)(Rom+1)\n");
   insert(&Intern, "T", romSym("T", "(Rom+5)"));
   fprintf(fp, "#define T (any)(Rom+5)\n");

   
   int f = FUNC_CTR++;
   functions[f] = (char*)malloc(200);
   sprintf(functions[f], "doQuote");
   sprintf(buf, "(num(0) + 2) /* %d */", f << 3);

   insert(&Intern, "quote", romSym("quote", buf));
   fprintf(fp, "#define Quote (any)(Rom+7)\nany doQuote(any);\n");
   do {
      if (!freopen(*++av, "r", stdin))
         giveup("Can't open input file");
      Chr = getchar();
      while ((x = read0(YES)) != Nil) {
         if (x & 2  ||  (x & 4) == 0)
            giveup("Symbol expected");
         if (skip() == '[') {                   // C Identifier
            fprintf(fp, "#define ");
            for (;;) {
               Chr = getchar();
               if (Chr == EOF)
                  break;
               if (Chr == ']') {
                  Chr = getchar();
                  break;
               }
               putc(Chr, fp);
            }
            print(buf, x, 3);
            fprintf(fp, " (any)%s\n", buf);
         }
         x >>= 2;
         if (skip() == '{') {                   // Function pointer
            for (p = Token;;) {
               Chr = getchar();
               if (Chr == EOF)
                  break;
               if (Chr == '}') {
                  Chr = getchar();
                  break;
               }
               *p++ = Chr;
            }
            *p = '\0';


	    f = FUNC_CTR++;
	    functions[f] = (char*)malloc(200);
	    sprintf(functions[f], "%s", Token);
            sprintf(buf, "(num(%d) + 2) /* %d */", (f << 3), f);
            Ram[-x] = strdup(buf);
            fprintf(fp, "any %s(any);\n", Token);
         }
         else {                                 // Value
         	 word val = read0(YES);
            print(buf, val, 4);
            if (x > 0)
			{
				addNumber(0, 1, RomIx - 2, val);
               Rom[x] = strdup(buf);
			}
            else
			{
				addNumber(1, 1, RamIx - 2, val);
               Ram[-x] = strdup(buf);
			}
         }
         while (skip() == ',') {                // Properties
            Chr = getchar();
            if (Chr == EOF)
               break;
            print(buf, read0(YES), 5);
            ix = RomIx;
            if (x > 0) {
               addList(&RomIx, &Rom, Rom[x-1], 0);
               addList(&RomIx, &Rom, buf, 0);
               print(buf, ix << 2, 6);
               Rom[x-1] = strdup(buf);
            }
            else {
               addList(&RomIx, &Rom, Ram[-x-1], 0);
               addList(&RomIx, &Rom, buf, 0);
               print(buf, ix << 2, 7);
               Ram[-x-1] = strdup(buf);
            }
         }
      }
   } while (--ac);
   fprintf(fp, "\n#define ROMS %d\n", RomIx);
   fprintf(fp, "#define RAMS %d\n", RamIx);
   fclose(fp);
   if (fp = fopen("rom.d", "w")) {
      for (x = 0; x < RomIx; x += 2)
         fprintf(fp, "(any)%s, (any)%s,\n", Rom[x], Rom[x+1]);
      fclose(fp);
   }
   if (fp = fopen("ram.d", "w")) {
      for (x = 0; x < RamIx; x += 2)
         fprintf(fp, "(any)%s, (any)%s,\n", Ram[x], Ram[x+1]);
      fclose(fp);
   }
   if (fp = fopen("functions.d", "w")) {
      for (x = 0; x < FUNC_CTR; x++)
         fprintf(fp, "(any)%s,\n", functions[x]);
      fclose(fp);
   }
   if (fp = fopen("numbers.d", "w")) {
   	   int numbersCount=0;
	   fprintf(fp, "any Numbers[] = {\n", numbersCount);
   	   while(NL != NULL)
       {
         fprintf(fp, "%s,\n", NL->address);
         fprintf(fp, "%d,\n", NL->value);
         NL=NL->next;
         numbersCount+=2;
       }
	   fprintf(fp, "};\n#define NUMBERS_COUNT (%d)", numbersCount);
      fclose(fp);
   }
   return 0;
}
