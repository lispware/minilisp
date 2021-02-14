#include "lisp.h"
#include "cell.h"


#if INTPTR_MAX == INT32_MAX
    #include "def32.d"
#elif INTPTR_MAX == INT64_MAX
    #include "def64.d"
#else
    #error "Unsupported bit width"
#endif


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

void comment(void)
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
