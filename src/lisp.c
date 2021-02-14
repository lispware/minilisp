
#include "lisp.h"

#include "cell.h"

#if INTPTR_MAX == INT32_MAX
    #include "def32.d"
#elif INTPTR_MAX == INT64_MAX
    #include "def64.d"
#else
    #error "Unsupported bit width"
#endif


bool isSym(any x)
{
   if (x) return 0;
   // TODO - this must be checked out
   return 0;
}


any evList(any);
any EVAL(any x)
{
   if (isNum(x))
   {
      return x;
   }
   else if (isTxt(x) || getCARType(x) == BIN_START)
   {
      return val(x);
   }
   else
   {
      return evList(x);
   }
}


static void gc(word c);


/* Globals */
int Chr, Trace;
char **AV, *AV0, *Home;
heap *Heaps;
cell *Avail;
stkEnv Env;
catchFrame *CatchPtr;
FILE *InFile, *OutFile;
any Intern[2], Transient[2];
any ApplyArgs, ApplyBody;

///////////////////////////////////////////////
//               sym.c
///////////////////////////////////////////////

int firstByte(any s)
{
    int c;

    if (isNil(s)) return 0;
    c = (uword)s;
    return c & 127;
}

int getByte1(int *i, uword *p, any *q)
{
    int c;

    if (getCARType(*q) == TXT)
    {
        (*q)=(*q)->car;
        *i = BITS, *p = (uword)(*q) , *q = NULL;
    }
    else if (getCARType(*q) == BIN_START)
    {
        (*q)=(*q)->car;
        *i = BITS, *p = (uword)((*q)->car) , *q = ((*q)->cdr);
    }
    else
    {
        giveup("Cant getByte");
    }

    c = *p & 127, *p >>= 8, *i -= 8;

    return c;
}

int getByte(int *i, uword *p, any *q)
{
    int c;

    if (*i == 0)
    {
        if (!*q)
        {
            return 0;
        }
        else
        {
            *i = BITS,  *p = (uword)((*q)->car),  *q = (*q)->cdr;
        }
    }
    c = *p & 127,  *p >>= 8;
    if (*i >= 8)
        *i -= 8;
    else if (isNum(*q))
    {
        *p = (uword)*q >> 2,  *q = NULL;
        c |= *p << *i;
        *p >>= 8 - *i;
        *i += BITS-9;
    }
    else
    {
        *p = (uword)tail(*q),  *q = val(*q);
        c |= *p << *i;
        *p >>= 8 - *i;
        *i += BITS-8;
    }
    c &= 127;

    return c;
}

any mkTxt(int c)
{
    return txt(c & 127);
}

any mkChar(int c)
{
   return consSym(NULL, c & 127);
}

void putByte1(int c, int *i, uword *p, any *q)
{
    *p = c & 127;
    *i = 8;
    *q = NULL;
}

void putByte(int c, int *i, uword *p, any *q, cell *cp)
{
    c = c & 127;
    int d = 8;

    if (*i != BITS)
        *p |= (uword)c << *i;

    if (*i + d  > BITS)
    {
        if (*q)
        {
            any x = consName(*p, Zero);
            setCARType(x, BIN);
            (*q)->cdr = x;
            *q = x;
        }
        else
        {
            any x = consSym(NULL, 0);
            setCARType(x, BIN_START);
            Push(*cp, x);
            any y = consName(*p, Zero);
            setCARType(y, BIN);
            (*cp).car->car = *q = y;

        }
        *p = c >> BITS - *i;
        *i -= BITS;
    }

    *i += d;
}

any popSym(int i, uword n, any q, cell *cp)
{
    if (q)
    {
        //val(q) = i <= (BITS-2)? box(n) : consName(n, Zero);
        q->cdr = consName(n, Nil);
        return Pop(*cp);
    }
    return consSym(NULL,n);
}

int symBytes(any x)
{
    int cnt = 0;
    uword w;

    if (isNil(x))
        return 0;

    if (isTxt(x))
    {
        w = (uword)(x->car);
        while (w)
        {
            ++cnt;
            w >>= 8;
        }
    }

    return cnt;
}

any isIntern(any nm, any tree[2])
{
    any x, y, z;
    word n;

    if (isTxt(nm))
    {
        for (x = tree[0];  x != Nil;)
        {
            if ((n = (word)(car(nm)) - (word)name(caar(x))) == 0)
            {
                return car(x);
            }
            x = n<0? cadr(x) : cddr(x);
        }
    }
    else
    {
        for (x = tree[1];  x != Nil;)
        {
            y = nm->car;
            z = x->car->car;
            while ((n = (word)(car(y)) - (word)car(z)) == 0)
            {
                if (getCARType(y) != BIN) return car(x);
                y=y->cdr;
                z=z->cdr;
            }

            x = n<0? cadr(x) : cddr(x);
        }
    }



    return NULL;
}

any internBin(any sym, any tree[2])
{
    any nm, x, y, z;
    word n;

    x = tree[1];

    if (x == Nil)
    {
        tree[1] = consIntern(sym, Nil);
        return tree[1];
    }

    for (;;)
    {

        y = sym->car;
        z = x->car->car;
        while ((n = (word)(car(y)) - (word)car(z)) == 0)
        {
            if (getCARType(y) != BIN) return sym;
            y=y->cdr;
            z=z->cdr;
        }

        if (Nil == cdr(x))
        {
            cdr(x) = n < 0 ? consIntern(consIntern(sym, Nil), Nil) : consIntern(Nil, consIntern(sym, Nil));
            return sym;
        }
        if (n < 0)
        {
            if (Nil != cadr(x))
            {
                x = cadr(x);
            }
            else
            {
                cadr(x) = consIntern(sym, Nil);
                return sym;
            }
        }
        else
        {
            if (Nil != cddr(x))
            {
                x = cddr(x);
            }
            else
            {
                cddr(x) = consIntern(sym, Nil);
                return sym;
            }
        }
    }
}

any intern(any sym, any tree[2])
{
   any nm, x;
   word n;


   if (getCARType(sym) == BIN_START) return internBin(sym, tree);

   nm = sym;

   x = tree[0];
   if (Nil == x)
   {
      tree[0] = consIntern(sym, Nil);
      return tree[0];
   }
   for (;;)
   {
      if ((n = (word)(car(nm)) - (word)name(caar(x))) == 0)
      {
         return car(x);
      }

      if (Nil == cdr(x))
      {
         cdr(x) = n < 0 ? consIntern(consIntern(sym, Nil), Nil) : consIntern(Nil, consIntern(sym, Nil));
         return sym;
      }
      if (n < 0)
      {
         if (Nil != cadr(x))
         {
            x = cadr(x);
         }
         else
         {
            cadr(x) = consIntern(sym, Nil);
            return sym;
         }
      }
      else
      {
         if (Nil != cddr(x))
         {
            x = cddr(x);
         }
         else
         {
            cddr(x) = consIntern(sym, Nil);
            return sym;
         }
      }
   }
}

/* Get symbol name */
any name(any s)
{
   return s;
}

/* Make name */
any mkSym(byte *s)
{
    int i;
    uword w;
    cell c1, *p;

    putByte1(*s++, &i, &w, &p);
    while (*s)
    {
        putByte(*s++, &i, &w, &p, &c1);
    }
    return popSym(i, w, p, &c1);
}

/* Make string */
any mkStr(char *s)
{
   if (s && *s)
   {
      return mkSym((byte *)s);
   }
   else
   {
      return Nil;
   }
}

// (==== ['sym ..]) -> NIL
any doHide(any ex)
{
    // TODO - is this needed?
   printf("%p\n", ex);

   return Nil;
}

void makeError(any ex) {err(ex, NULL, "Not making");}


///////////////////////////////////////////////
//               sym.c - END
///////////////////////////////////////////////


///////////////////////////////////////////////
//               io.c - START
///////////////////////////////////////////////

static any read0(bool);

static char Delim[] = " \t\n\r\"'(),[]`~{}";

/* Buffer size */
int bufSize(any x)
{
    return symBytes(x) + 1;
}

int pathSize(any x)
{
    int c = firstByte(x);

    if (c != '@'  &&  (c != '+'))
    {
        return bufSize(x);
    }
    if (!Home)
    {
        return symBytes(x);
    }
    return strlen(Home) + symBytes(x);
}

void bufString(any x, char *p)
{
    int c, i;
    uword w;

    if (!isNil(x))
    {
        for (x = name(x), c = getByte1(&i, &w, &x); c; c = getByte(&i, &w, &x))
        {
            if (c == '^')
            {
                if ((c = getByte(&i, &w, &x)) == '?')
                {
                    c = 127;
                }
                else
                {
                    c &= 0x1F;
                }
            }
            *p++ = c;
        }
    }
    *p = '\0';
}


/*** Reading ***/
void getStdin(void)
{
    Chr = getc(InFile);
}


/* Skip White Space and Comments */
static int skipc(int c)
{
    if (Chr < 0)
    {
        return Chr;
    }
    for (;;)
    {
        while (Chr <= ' ')
        {
            Env.get();
            if (Chr < 0)
            {
                return Chr;
            }
        }
        if (Chr != c)
        {
            return Chr;
        }
        Env.get();
        while (Chr != '\n')
        {
            if (Chr < 0)
            {
                return Chr;
            }
            Env.get();
        }
    }
}

static void comment(void)
{
    Env.get();
    if (Chr != '{')
    {
        while (Chr != '\n')
        {
            if (Chr < 0)
            {
                return;
            }
            Env.get();
        }
    }
    else
    {
        int n = 0;

        for (;;) {  // #{block-comment}# from Kriangkrai Soatthiyanont
            Env.get();
            if (Chr < 0)
            {
                return;
            }
            if (Chr == '#'  &&  (Env.get(), Chr == '{'))
            {
                ++n;
            }
            else if (Chr == '}'  &&  (Env.get(), Chr == '#')  &&  --n < 0)
            {
                break;
            }
        }
        Env.get();
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
            Env.get();
            if (Chr < 0)
            {
                return Chr;
            }
        }

        if (Chr != '#')
        {
            return Chr;
        }
        comment();
    }
}

/* Test for escaped characters */
static bool testEsc(void)
{
    for (;;)
    {
        if (Chr < 0)
            return NO;
        if (Chr != '\\')
            return YES;
        if (Env.get(), Chr != '\n')
            return YES;
        do
        {
            Env.get();
        }
        while (Chr == ' '  ||  Chr == '\t');
    }
}

/* Read a list */
static any rdList(void)
{
    any x;
    cell c1;

    for (;;)
    {
        if (skip() == ')')
        {
            Env.get();
            return Nil;
        }
        if (Chr == ']')
        {
            return Nil;
        }
        if (Chr != '~')
        {
            x = cons(read0(NO),Nil);
            Push(c1, x);
            break;
        }
        Env.get();

        x = read0(NO);
        Push(c1, x);
        if (isCell(x = data(c1) = EVAL(data(c1))))
        {
            while (isCell(cdr(x)))
            {
                x = cdr(x);
            }
            break;
        }
        drop(c1);
    }

    for (;;)
    {
        if (skip() == ')')
        {
            Env.get();
            break;
        }
        if (Chr == ']')
            break;
        if (Chr == '.')
        {
            Env.get();
            cdr(x) = skip()==')' || Chr==']'? data(c1) : read0(NO);
            if (skip() == ')')
                Env.get();
            else if (Chr != ']')
                err(NULL, x, "Bad dotted pair");
            break;
        }
        if (Chr != '~')
        {
            x = cdr(x) = cons(read0(NO),Nil);
        }
        else
        {
            Env.get();
            cdr(x) = read0(NO);
            cdr(x) = EVAL(cdr(x));
            while (isCell(cdr(x)))
            {
                x = cdr(x);
            }
        }
    }
    return Pop(c1);
}

/* Read one expression */
static any read0(bool top)
{
    int i;
    uword w;
    any x, y;
    cell c1, *p;

    if (skip() < 0)
    {
        if (top)
            return Nil;
        eofErr();
    }
    if (Chr == '(')
    {
        Env.get();
        x = rdList();
        if (top  &&  Chr == ']')
            Env.get();
        return x;
    }
    if (Chr == '[')
    {
        Env.get();
        x = rdList();
        if (Chr != ']')
            err(NULL, x, "Super parentheses mismatch");
        Env.get();
        return x;
    }
    if (Chr == '\'')
    {
        Env.get();
        return cons(doQuote_D, read0(top));
    }
    if (Chr == ',')
    {
        Env.get();
        return read0(top);
    }
    if (Chr == '`')
    {
        Env.get();
        Push(c1, read0(top));
        x = EVAL(data(c1));
        drop(c1);
        return x;
    }
    if (Chr == '"')
    {
        Env.get();
        if (Chr == '"')
        {
            Env.get();
            return Nil;
        }
        if (!testEsc())
            eofErr();
        putByte1(Chr, &i, &w, &p);
        while (Env.get(), Chr != '"')
        {
            if (!testEsc())
                eofErr();
            putByte(Chr, &i, &w, &p, &c1);
        }
        y = popSym(i, w, p, &c1),  Env.get();
        if (x = isIntern(tail(y), Transient))
            return x;
        intern(y, Transient);
        return y;
    }
    if (strchr(Delim, Chr))
        err(NULL, NULL, "Bad input '%c' (%d)", isprint(Chr)? Chr:'?', Chr);
    if (Chr == '\\')
        Env.get();
    putByte1(Chr, &i, &w, &p);

    int count=0;
    for (;;)
    {
        count++;
        // if (count > 6)
        // {
        //     printf("%s too long\n", (char*)&w);
        //     bye(0);
        // }
        Env.get();
        if (strchr(Delim, Chr))
        {
            break;
        }
        if (Chr == '\\')
        {
            Env.get();
        }
        putByte(Chr, &i, &w, &p, &c1);
    }

    y = popSym(i, w, p, &c1);
    //printf("%p --> CAR = %p CDR = %p \n", y, y->car, y->cdr);
    if (x = symToNum(tail(y), 0, '.', 0))
    {
        return x;
    }
    if (x = isIntern(tail(y), Intern))
    {
        return x;
    }

    intern(y, Intern);
    //val(y) = Nil;
    return y;
}

any read1(int end)
{
   if (!Chr)
      Env.get();
   if (Chr == end)
      return Nil;
   return read0(YES);
}

/* Read one token */
any token(any x, int c)
{
    int i;
    uword w;
    any y;
    cell c1, *p;

    if (!Chr)
        Env.get();
    if (skipc(c) < 0)
        return Nil;
    if (Chr == '"')
    {
        Env.get();
        if (Chr == '"')
        {
            Env.get();
            return Nil;
        }
        if (!testEsc())
            return Nil;
        Push(c1, y =  cons(mkChar(Chr), Nil));
        while (Env.get(), Chr != '"' && testEsc())
            y = cdr(y) = cons(mkChar(Chr), Nil);
        Env.get();
        return Pop(c1);
    }
    if (Chr >= '0' && Chr <= '9')
    {
        putByte1(Chr, &i, &w, &p);
        while (Env.get(), Chr >= '0' && Chr <= '9' || Chr == '.')
            putByte(Chr, &i, &w, &p, &c1);
        return symToNum(tail(popSym(i, w, p, &c1)), 0, '.', 0);
    }
    if (Chr != '+' && Chr != '-')
    {
        // TODO check what needs to be done about stack - FREE MUST BE ADDED
        // char nm[bufSize(x)];
        char *nm = (char *)malloc(bufSize(x));

        bufString(x, nm);
        if (Chr >= 'A' && Chr <= 'Z' || Chr == '\\' || Chr >= 'a' && Chr <= 'z' || strchr(nm,Chr))
        {
            if (Chr == '\\')
                Env.get();
            putByte1(Chr, &i, &w, &p);
            while (Env.get(),
                    Chr >= '0' && Chr <= '9' || Chr >= 'A' && Chr <= 'Z' ||
                    Chr == '\\' || Chr >= 'a' && Chr <= 'z' || strchr(nm,Chr) )
            {
                if (Chr == '\\')
                    Env.get();
                putByte(Chr, &i, &w, &p, &c1);
            }
            y = popSym(i, w, p, &c1);
            if (x = isIntern(tail(y), Intern))
            {
                free(nm);
                return x;
            }
            intern(y, Intern);
            val(y) = Nil;
            free(nm);
            return y;
        }
    }
    y = mkTxt(c = Chr);
    Env.get();
    return mkChar(c);
}

bool eol(void)
{
   if (Chr < 0)
      return YES;
   if (Chr == '\n')
   {
      Chr = 0;
      return YES;
   }
   if (Chr == '\r')
   {
      Env.get();
      if (Chr == '\n')
         Chr = 0;
      return YES;
   }
   return NO;
}

any load(any ex, int pr, any x)
{
    cell c1, c2;
    inFrame f;

    // TODO - get back function execution from command line if (isSymb(x) && firstByte(x) == '-')

    rdOpen(ex, x, &f);
    pushInFiles(&f);
    //doHide(Nil);
    x = Nil;
    for (;;)
    {
        if (InFile != stdin)
        {
            data(c1) = read1(0);
        }
        else
        {
            if (pr && !Chr)
                Env.put(pr), space(), fflush(OutFile);
            data(c1) = read1('\n');
            while (Chr > 0)
            {
                if (Chr == '\n')
                {
                    Chr = 0;
                    break;
                }
                if (Chr == '#')
                    comment();
                else
                {
                    if (Chr > ' ')
                        break;
                    Env.get();
                }
            }
        }
        if (isNil(data(c1)))
        {
            popInFiles();
            doHide(Nil);
            return x;
        }
        Save(c1);
        if (InFile != stdin || Chr || !pr)
            // TODO - WHY @ does not work in files
            x = EVAL(data(c1));
        else
        {
            Push(c2, val(At));
            x = EVAL(data(c1));
            cdr(At) = x;
            setCDRType(At, getCDRType(x));
            //x = val(At) = EVAL(data(c1));

            cdr(At2) = c2.car;
            setCDRType(At2, getCARType(&c2));

            cdr(At3) = cdr(At2);
            setCDRType(At3, getCDRType(At2));

            //val(At3) = val(At2),  val(At2) = data(c2);
            outString("-> "),  fflush(OutFile),  print(x),  newline();
        }
        drop(c1);
    }
}

/*** Prining ***/
void putStdout(int c)
{
    putc(c, OutFile);
}

void newline(void)
{
    Env.put('\n');
}

void space(void)
{
    Env.put(' ');
}

void outString(char *s)
{
    while (*s)
        Env.put(*s++);
}

int bufNum(char buf[BITS/2], word n)
{
    return sprintf(buf, WORD_FORMAT_STRING_D, n); // TODO - this is not quite right for 32 bit
}

void outNum(word n)
{
    char buf[BITS/2];

    bufNum(buf, n);
    outString(buf);
}

/* Print one expression */
void print(any x)
{
    if (x == T)
    {
        printf("T");
        return;
    }

    if (x == Nil)
    {
        printf("Nil");
        return;
    }

    if (isNum(x))
    {
        outNum(unBox(x));
        return;
    }
    if (getCARType(x) == TXT)
    {
        printf("%s", (char*)&x->car);
        return;
    }
    if (getCARType(x) == BIN_START)
    {
        printLongTXT(x);
        return;
    }

    if (getCARType(x) == PTR_CELL && getCDRType(x) == PTR_CELL)
    {
        printf("(");
        print(x->car);
        while (x != Nil)
        {
            x = x->cdr;
            if (x->car != Nil)
            {
                printf(" ");
                print(x->car);
            }
        }
        printf(")");
        return;
    }

    printf ("TODO NOT A NUMBER %p %p\n", x, Nil);
    return;
}

void prin(any x)
{
    if (x == Nil)
    {
        printf("Nil");
        return;
    }

    if (!isNil(x))
    {
        if (isNum(x))
        {
            outNum(unBox(x));
        }
        else if (x == T)
        {
            printf("T");
        }
        else if (getCARType(x) == TXT)
        {
            printLongTXT(x);
        }
        else if (getCARType(x) == BIN_START)
        {
            printLongTXT(x);

        }
        else if (isSym(x))
        {
            int i, c;
            uword w;
            while(1);

            for (x = name(x), c = getByte1(&i, &w, &x); c; c = getByte(&i, &w, &x))
            {
                if (c != '^')
                    Env.put(c);
                else if (!(c = getByte(&i, &w, &x)))
                    Env.put('^');
                else if (c == '?')
                    Env.put(127);
                else
                    Env.put(c &= 0x1F);
            }
        }
        else
        {
            while (prin(car(x)), !isNil(x = cdr(x)))
            {
                if (!isCell(x))
                {
                    prin(x);
                    break;
                }
            }
        }
    }
}


///////////////////////////////////////////////
//               io.c - END
///////////////////////////////////////////////


///////////////////////////////////////////////
//               math.c START
///////////////////////////////////////////////



///////////////////////////////////////////////
//               math.c END
///////////////////////////////////////////////


///////////////////////////////////////////////
//               flow.c START
///////////////////////////////////////////////





///////////////////////////////////////////////
//               flow.c END
///////////////////////////////////////////////


///////////////////////////////////////////////
//               gc.c START
///////////////////////////////////////////////


///////////////////////////////////////////////
//               gc.c END
///////////////////////////////////////////////

/*** System ***/
void giveup(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

void bye(int n)
{
    exit(n);
}

/* Allocate memory */
void *alloc(void *p, size_t siz)
{
   if (!(p = realloc(p,siz)))
      giveup("No memory");
   return p;
}


/*** Error handling ***/
void err(any ex, any x, char *fmt, ...)
{
    printf("ERROR\n");
    bye(0);
    if (ex == x) bye(1);
    if (fmt == NULL) bye(1);
}

void argError(any ex, any x) {err(ex, x, "Bad argument");}
void numError(any ex, any x) {err(ex, x, "Number expected");}
void symError(any ex, any x) {err(ex, x, "Symbol expected");}
void pairError(any ex, any x) {err(ex, x, "Cons pair expected");}
void atomError(any ex, any x) {err(ex, x, "Atom expected");}
void lstError(any ex, any x) {err(ex, x, "List expected");}
void varError(any ex, any x) {err(ex, x, "Variable expected");}
void protError(any ex, any x) {err(ex, x, "Protected symbol");}

/*** Evaluation ***/
any evExpr(any expr, any x)
{
   any y = car(expr);

   bindFrame *f = allocFrame(length(y)+2);

   f->link = Env.bind,  Env.bind = f;
   f->i = (bindSize * (length(y)+2)) / (2*sizeof(any)) - 1;
   f->cnt = 1,  f->bnd[0].sym = At,  f->bnd[0].val = val(At);

   while (y != Nil && y != cdr(y) && 0 != cdr(y))
   {
      f->bnd[f->cnt].sym = car(y);
      f->bnd[f->cnt].val = EVAL(car(x));
      ++f->cnt;
      x = cdr(x);
      y = cdr(y);
   }

   if (isNil(y)) {
      do
      {
         x = val(f->bnd[--f->i].sym);
         val(f->bnd[f->i].sym) = f->bnd[f->i].val;
         f->bnd[f->i].val = x;
      }
      while (f->i);

      x = prog(cdr(expr));
   }
   else if (y != At)
   {
      f->bnd[f->cnt].sym = y,  f->bnd[f->cnt++].val = val(y),  val(y) = x;
      do
      {
         x = val(f->bnd[--f->i].sym);
         val(f->bnd[f->i].sym) = f->bnd[f->i].val;
         f->bnd[f->i].val = x;
      }
      while (f->i);
      x = prog(cdr(expr));
   }
   else
   {
      int n, cnt;
      cell *arg;
      cell *c = (cell*)malloc(sizeof(cell) * (n = cnt = length(x)));

      while (--n >= 0)
      {
         Push(c[n], EVAL(car(x))),  x = cdr(x);
      }

      do
      {
         x = val(f->bnd[--f->i].sym);
         val(f->bnd[f->i].sym) = f->bnd[f->i].val;
         f->bnd[f->i].val = x;
      }
      while (f->i);

      n = Env.next,  Env.next = cnt;
      arg = Env.arg,  Env.arg = c;
      x = prog(cdr(expr));
      if (cnt)
      {
         drop(c[cnt-1]);
      }

      Env.arg = arg,  Env.next = n;
      free(c);
   }

   while (--f->cnt >= 0)
   {
      val(f->bnd[f->cnt].sym) = f->bnd[f->cnt].val;
   }

   Env.bind = f->link;
   free(f);
   return x;
}

void undefined(any x, any ex) {err(ex, x, "Undefined");}

static any evList2(any foo, any ex)
{
    cell c1;

    Push(c1, foo);
    if (isCell(foo))
    {
        foo = evExpr(foo, cdr(ex));
        drop(c1);
        return foo;
    }
    for (;;)
    {
        if (isNil(val(foo)))
            undefined(foo,ex);
        if (isNum(foo = val(foo)))
        {
            foo = evSubr(foo,ex);
            drop(c1);
            return foo;
        }
        if (isCell(foo))
        {
            foo = evExpr(foo, cdr(ex));
            drop(c1);
            return foo;
        }
    }
}

/* Evaluate a list */
any evList(any ex)
{
    any foo;

    if (ex == Nil) return Nil;

    if (isNum(foo = car(ex)))
        return ex;

    if (getCARType(ex) == BIN_START) return Nil;

    if (isCell(foo))
    {
        if (isNum(foo = evList(foo)))
            return evSubr(foo,ex);
        return evList2(foo,ex);
    }
    for (;;)
    {
        if (isNil(val(foo)))
            undefined(foo,ex);
        if (isFunc(foo))
            return evSubr(foo->cdr,ex);
        if (isNum(foo = val(foo)))
            return evSubr(foo,ex);
        if (isCell(foo))
            return evExpr(foo, cdr(ex));
    }
}

any loadAll(any ex)
{
   any x = Nil;

   while (*AV  &&  strcmp(*AV,"-") != 0)
      x = load(ex, 0, mkStr(*AV++));
   return x;
}

void printLongTXT(any nm)
{
    int i, c;
    word w;

    c = getByte1(&i, &w, &nm);
    do
    {
        if (c == '"'  ||  c == '\\')
        {
            Env.put('\\');
        }
        Env.put(c);
    }
   while (c = getByte(&i, &w, &nm));
}

void printTXT(any cell)
{
    printLongTXT(cell);
}

void printNUM(any cell)
{
    printf(WORD_FORMAT_STRING_D, (word)cell->car);
}

/*** Main ***/
int main(int ac, char *av[])
{
   if (ac == 0) printf("STRANGE\n");

   av++;
   AV = av;
   heapAlloc();
   doDump(Nil);
   //getHeapSize();
   //CELLS = 1;
   Intern[0] = Intern[1] = Transient[0] = Transient[1] = Nil;

   Mem[4] = (any)Mem; // TODO - SETTING THE VALUE OF NIL
   Mem[7] = (any)(Mem+6); // TODO - SETTING THE VALUE OF NIL

   for (int i = 3; i < MEMS; i += 3) // 2 because Nil has already been interned
   {
      any cell = (any)&Mem[i];
      CellPartType carType = getCARType(cell);
      CellPartType cdrType = getCDRType(cell);

      if ((BIN_START == carType || TXT == carType) && cdrType != FUNC && cell->cdr)
      {
         intern(cell, Intern);
      }
      else if ((BIN_START == carType || TXT == carType) && cdrType == FUNC && cell->cdr)
      {
         intern(cell, Intern);
      }
      else if ((BIN_START == carType || TXT == carType))
      {
         intern(cell, Intern);
      }
   }

   InFile = stdin, Env.get = getStdin;
   OutFile = stdout, Env.put = putStdout;
   ApplyArgs = cons(cons(consSym(Nil, 0), Nil), Nil);
   ApplyBody = cons(Nil, Nil);

   doDump(Nil);
   //getHeapSize();
   loadAll(NULL);
   while (!feof(stdin))
      load(NULL, ':', Nil);
   bye(0);
}
