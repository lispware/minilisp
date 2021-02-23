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
    if (!CONTEXT.Home)
    {
        return symBytes(x);
    }
    return strlen(CONTEXT.Home) + symBytes(x);
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
    CONTEXT.Chr = getc(CONTEXT.InFile);
}


/* Skip White Space and Comments */
static int skipc(int c)
{
    if (CONTEXT.Chr < 0)
    {
        return CONTEXT.Chr;
    }
    for (;;)
    {
        while (CONTEXT.Chr <= ' ')
        {
            CONTEXT.Env.get();
            if (CONTEXT.Chr < 0)
            {
                return CONTEXT.Chr;
            }
        }
        if (CONTEXT.Chr != c)
        {
            return CONTEXT.Chr;
        }
        CONTEXT.Env.get();
        while (CONTEXT.Chr != '\n')
        {
            if (CONTEXT.Chr < 0)
            {
                return CONTEXT.Chr;
            }
            CONTEXT.Env.get();
        }
    }
}

void comment(void)
{
    CONTEXT.Env.get();
    if (CONTEXT.Chr != '{')
    {
        while (CONTEXT.Chr != '\n')
        {
            if (CONTEXT.Chr < 0)
            {
                return;
            }
            CONTEXT.Env.get();
        }
    }
    else
    {
        int n = 0;

        for (;;) {  // #{block-comment}# from Kriangkrai Soatthiyanont
            CONTEXT.Env.get();
            if (CONTEXT.Chr < 0)
            {
                return;
            }
            if (CONTEXT.Chr == '#'  &&  (CONTEXT.Env.get(), CONTEXT.Chr == '{'))
            {
                ++n;
            }
            else if (CONTEXT.Chr == '}'  &&  (CONTEXT.Env.get(), CONTEXT.Chr == '#')  &&  --n < 0)
            {
                break;
            }
        }
        CONTEXT.Env.get();
    }
}

static int skip(void)
{
    for (;;)
    {
        if (CONTEXT.Chr < 0)
        {
            return CONTEXT.Chr;
        }
        while (CONTEXT.Chr <= ' ')
        {
            CONTEXT.Env.get();
            if (CONTEXT.Chr < 0)
            {
                return CONTEXT.Chr;
            }
        }

        if (CONTEXT.Chr != '#')
        {
            return CONTEXT.Chr;
        }
        comment();
    }
}

/* Test for escaped characters */
static bool testEsc(void)
{
    for (;;)
    {
        if (CONTEXT.Chr < 0)
            return NO;
        if (CONTEXT.Chr != '\\')
            return YES;
        if (CONTEXT.Env.get(), CONTEXT.Chr != '\n')
            return YES;
        do
        {
            CONTEXT.Env.get();
        }
        while (CONTEXT.Chr == ' '  ||  CONTEXT.Chr == '\t');
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
            CONTEXT.Env.get();
            return Nil;
        }
        if (CONTEXT.Chr == ']')
        {
            return Nil;
        }
        if (CONTEXT.Chr != '~')
        {
            x = cons(read0(NO),Nil);
            Push(c1, x);
            break;
        }
        CONTEXT.Env.get();

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
            CONTEXT.Env.get();
            break;
        }
        if (CONTEXT.Chr == ']')
            break;
        if (CONTEXT.Chr == '.')
        {
            CONTEXT.Env.get();
            cdr(x) = skip()==')' || CONTEXT.Chr==']'? data(c1) : read0(NO);
            if (skip() == ')')
                CONTEXT.Env.get();
            else if (CONTEXT.Chr != ']')
                err(NULL, x, "Bad dotted pair");
            break;
        }
        if (CONTEXT.Chr != '~')
        {
            x = cdr(x) = cons(read0(NO),Nil);
        }
        else
        {
            CONTEXT.Env.get();
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
    if (CONTEXT.Chr == '(')
    {
        CONTEXT.Env.get();
        x = rdList();
        if (top  &&  CONTEXT.Chr == ']')
            CONTEXT.Env.get();
        return x;
    }
    if (CONTEXT.Chr == '[')
    {
        CONTEXT.Env.get();
        x = rdList();
        if (CONTEXT.Chr != ']')
            err(NULL, x, "Super parentheses mismatch");
        CONTEXT.Env.get();
        return x;
    }
    if (CONTEXT.Chr == '\'')
    {
        CONTEXT.Env.get();
        return cons(doQuote_D, read0(top));
    }
    if (CONTEXT.Chr == ',')
    {
        CONTEXT.Env.get();
        return read0(top);
    }
    if (CONTEXT.Chr == '`')
    {
        CONTEXT.Env.get();
        Push(c1, read0(top));
        x = EVAL(data(c1));
        drop(c1);
        return x;
    }
    if (CONTEXT.Chr == '"')
    {
        CONTEXT.Env.get();
        if (CONTEXT.Chr == '"')
        {
            CONTEXT.Env.get();
            return Nil;
        }
        if (!testEsc())
            eofErr();
        putByte1(CONTEXT.Chr, &i, &w, &p);
        while (CONTEXT.Env.get(), CONTEXT.Chr != '"')
        {
            if (!testEsc())
                eofErr();
            putByte(CONTEXT.Chr, &i, &w, &p, &c1);
        }
        y = popSym(i, w, p, &c1),  CONTEXT.Env.get();
        if (x = isIntern(tail(y), CONTEXT.Transient))
            return x;
        intern(y, CONTEXT.Transient);
        return y;
    }
    if (strchr(Delim, CONTEXT.Chr))
        err(NULL, NULL, "Bad input '%c' (%d)", isprint(CONTEXT.Chr)? CONTEXT.Chr:'?', CONTEXT.Chr);
    if (CONTEXT.Chr == '\\')
        CONTEXT.Env.get();
    putByte1(CONTEXT.Chr, &i, &w, &p);

    int count=0;
    for (;;)
    {
        count++;
        // if (count > 6)
        // {
        //     printf("%s too long\n", (char*)&w);
        //     bye(0);
        // }
        CONTEXT.Env.get();
        if (strchr(Delim, CONTEXT.Chr))
        {
            break;
        }
        if (CONTEXT.Chr == '\\')
        {
            CONTEXT.Env.get();
        }
        putByte(CONTEXT.Chr, &i, &w, &p, &c1);
    }

    y = popSym(i, w, p, &c1);
    //printf("%p --> CAR = %p CDR = %p \n", y, y->car, y->cdr);
    if (x = symToNum(tail(y), 0, '.', 0))
    {
        return x;
    }
    if (x = isIntern(tail(y), CONTEXT.Intern))
    {
        return x;
    }

    intern(y, CONTEXT.Intern);
    //val(y) = Nil;
    return y;
}

any read1(int end)
{
   if (!CONTEXT.Chr)
      CONTEXT.Env.get();
   if (CONTEXT.Chr == end)
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

    if (!CONTEXT.Chr)
        CONTEXT.Env.get();
    if (skipc(c) < 0)
        return Nil;
    if (CONTEXT.Chr == '"')
    {
        CONTEXT.Env.get();
        if (CONTEXT.Chr == '"')
        {
            CONTEXT.Env.get();
            return Nil;
        }
        if (!testEsc())
            return Nil;
        Push(c1, y =  cons(mkChar(CONTEXT.Chr), Nil));
        while (CONTEXT.Env.get(), CONTEXT.Chr != '"' && testEsc())
            y = cdr(y) = cons(mkChar(CONTEXT.Chr), Nil);
        CONTEXT.Env.get();
        return Pop(c1);
    }
    if (CONTEXT.Chr >= '0' && CONTEXT.Chr <= '9')
    {
        putByte1(CONTEXT.Chr, &i, &w, &p);
        while (CONTEXT.Env.get(), CONTEXT.Chr >= '0' && CONTEXT.Chr <= '9' || CONTEXT.Chr == '.')
            putByte(CONTEXT.Chr, &i, &w, &p, &c1);
        return symToNum(tail(popSym(i, w, p, &c1)), 0, '.', 0);
    }
    if (CONTEXT.Chr != '+' && CONTEXT.Chr != '-')
    {
        // TODO check what needs to be done about stack - FREE MUST BE ADDED
        // char nm[bufSize(x)];
        char *nm = (char *)malloc(bufSize(x));

        bufString(x, nm);
        if (CONTEXT.Chr >= 'A' && CONTEXT.Chr <= 'Z' || CONTEXT.Chr == '\\' || CONTEXT.Chr >= 'a' && CONTEXT.Chr <= 'z' || strchr(nm,CONTEXT.Chr))
        {
            if (CONTEXT.Chr == '\\')
                CONTEXT.Env.get();
            putByte1(CONTEXT.Chr, &i, &w, &p);
            while (CONTEXT.Env.get(),
                    CONTEXT.Chr >= '0' && CONTEXT.Chr <= '9' || CONTEXT.Chr >= 'A' && CONTEXT.Chr <= 'Z' ||
                    CONTEXT.Chr == '\\' || CONTEXT.Chr >= 'a' && CONTEXT.Chr <= 'z' || strchr(nm,CONTEXT.Chr) )
            {
                if (CONTEXT.Chr == '\\')
                    CONTEXT.Env.get();
                putByte(CONTEXT.Chr, &i, &w, &p, &c1);
            }
            y = popSym(i, w, p, &c1);
            if (x = isIntern(tail(y), CONTEXT.Intern))
            {
                free(nm);
                return x;
            }
            intern(y, CONTEXT.Intern);
            val(y) = Nil;
            free(nm);
            return y;
        }
    }
    y = mkTxt(c = CONTEXT.Chr);
    CONTEXT.Env.get();
    return mkChar(c);
}

bool eol(void)
{
   if (CONTEXT.Chr < 0)
      return YES;
   if (CONTEXT.Chr == '\n')
   {
      CONTEXT.Chr = 0;
      return YES;
   }
   if (CONTEXT.Chr == '\r')
   {
      CONTEXT.Env.get();
      if (CONTEXT.Chr == '\n')
         CONTEXT.Chr = 0;
      return YES;
   }
   return NO;
}
