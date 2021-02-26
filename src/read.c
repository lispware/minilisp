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
    if (!CONTEXT_PTR->Home)
    {
        return symBytes(x);
    }
    return strlen(CONTEXT_PTR->Home) + symBytes(x);
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
    CONTEXT_PTR->Chr = getc(CONTEXT_PTR->InFile);
}


/* Skip White Space and Comments */
static int skipc(int c)
{
    if (CONTEXT_PTR->Chr < 0)
    {
        return CONTEXT_PTR->Chr;
    }
    for (;;)
    {
        while (CONTEXT_PTR->Chr <= ' ')
        {
            CONTEXT_PTR->Env.get();
            if (CONTEXT_PTR->Chr < 0)
            {
                return CONTEXT_PTR->Chr;
            }
        }
        if (CONTEXT_PTR->Chr != c)
        {
            return CONTEXT_PTR->Chr;
        }
        CONTEXT_PTR->Env.get();
        while (CONTEXT_PTR->Chr != '\n')
        {
            if (CONTEXT_PTR->Chr < 0)
            {
                return CONTEXT_PTR->Chr;
            }
            CONTEXT_PTR->Env.get();
        }
    }
}

void comment(void)
{
    CONTEXT_PTR->Env.get();
    if (CONTEXT_PTR->Chr != '{')
    {
        while (CONTEXT_PTR->Chr != '\n')
        {
            if (CONTEXT_PTR->Chr < 0)
            {
                return;
            }
            CONTEXT_PTR->Env.get();
        }
    }
    else
    {
        int n = 0;

        for (;;) {  // #{block-comment}# from Kriangkrai Soatthiyanont
            CONTEXT_PTR->Env.get();
            if (CONTEXT_PTR->Chr < 0)
            {
                return;
            }
            if (CONTEXT_PTR->Chr == '#'  &&  (CONTEXT_PTR->Env.get(), CONTEXT_PTR->Chr == '{'))
            {
                ++n;
            }
            else if (CONTEXT_PTR->Chr == '}'  &&  (CONTEXT_PTR->Env.get(), CONTEXT_PTR->Chr == '#')  &&  --n < 0)
            {
                break;
            }
        }
        CONTEXT_PTR->Env.get();
    }
}

static int skip(void)
{
    for (;;)
    {
        if (CONTEXT_PTR->Chr < 0)
        {
            return CONTEXT_PTR->Chr;
        }
        while (CONTEXT_PTR->Chr <= ' ')
        {
            CONTEXT_PTR->Env.get();
            if (CONTEXT_PTR->Chr < 0)
            {
                return CONTEXT_PTR->Chr;
            }
        }

        if (CONTEXT_PTR->Chr != '#')
        {
            return CONTEXT_PTR->Chr;
        }
        comment();
    }
}

/* Test for escaped characters */
static bool testEsc(void)
{
    for (;;)
    {
        if (CONTEXT_PTR->Chr < 0)
            return NO;
        if (CONTEXT_PTR->Chr != '\\')
            return YES;
        if (CONTEXT_PTR->Env.get(), CONTEXT_PTR->Chr != '\n')
            return YES;
        do
        {
            CONTEXT_PTR->Env.get();
        }
        while (CONTEXT_PTR->Chr == ' '  ||  CONTEXT_PTR->Chr == '\t');
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
            CONTEXT_PTR->Env.get();
            return Nil;
        }
        if (CONTEXT_PTR->Chr == ']')
        {
            return Nil;
        }
        if (CONTEXT_PTR->Chr != '~')
        {
            x = cons(read0(NO),Nil);
            Push(c1, x);
            break;
        }
        CONTEXT_PTR->Env.get();

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
            CONTEXT_PTR->Env.get();
            break;
        }
        if (CONTEXT_PTR->Chr == ']')
            break;
        if (CONTEXT_PTR->Chr == '.')
        {
            CONTEXT_PTR->Env.get();
            cdr(x) = skip()==')' || CONTEXT_PTR->Chr==']'? data(c1) : read0(NO);
            if (skip() == ')')
                CONTEXT_PTR->Env.get();
            else if (CONTEXT_PTR->Chr != ']')
                err(NULL, x, "Bad dotted pair");
            break;
        }
        if (CONTEXT_PTR->Chr != '~')
        {
            x = cdr(x) = cons(read0(NO),Nil);
        }
        else
        {
            CONTEXT_PTR->Env.get();
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
    if (CONTEXT_PTR->Chr == '(')
    {
        CONTEXT_PTR->Env.get();
        x = rdList();
        if (top  &&  CONTEXT_PTR->Chr == ']')
            CONTEXT_PTR->Env.get();
        return x;
    }
    if (CONTEXT_PTR->Chr == '[')
    {
        CONTEXT_PTR->Env.get();
        x = rdList();
        if (CONTEXT_PTR->Chr != ']')
            err(NULL, x, "Super parentheses mismatch");
        CONTEXT_PTR->Env.get();
        return x;
    }
    if (CONTEXT_PTR->Chr == '\'')
    {
        CONTEXT_PTR->Env.get();
        return cons(doQuote_D, read0(top));
    }
    if (CONTEXT_PTR->Chr == ',')
    {
        CONTEXT_PTR->Env.get();
        return read0(top);
    }
    if (CONTEXT_PTR->Chr == '`')
    {
        CONTEXT_PTR->Env.get();
        Push(c1, read0(top));
        x = EVAL(data(c1));
        drop(c1);
        return x;
    }
    if (CONTEXT_PTR->Chr == '"')
    {
        CONTEXT_PTR->Env.get();
        if (CONTEXT_PTR->Chr == '"')
        {
            CONTEXT_PTR->Env.get();
            return Nil;
        }
        if (!testEsc())
            eofErr();
        putByte1(CONTEXT_PTR->Chr, &i, &w, &p);
        while (CONTEXT_PTR->Env.get(), CONTEXT_PTR->Chr != '"')
        {
            if (!testEsc())
                eofErr();
            putByte(CONTEXT_PTR->Chr, &i, &w, &p, &c1);
        }
        y = popSym(i, w, p, &c1),  CONTEXT_PTR->Env.get();
        if (x = isIntern(tail(y), CONTEXT_PTR->Transient))
            return x;
        intern(y, CONTEXT_PTR->Transient);
        return y;
    }
    if (strchr(Delim, CONTEXT_PTR->Chr))
        err(NULL, NULL, "Bad input '%c' (%d)", isprint(CONTEXT_PTR->Chr)? CONTEXT_PTR->Chr:'?', CONTEXT_PTR->Chr);
    if (CONTEXT_PTR->Chr == '\\')
        CONTEXT_PTR->Env.get();
    putByte1(CONTEXT_PTR->Chr, &i, &w, &p);

    int count=0;
    for (;;)
    {
        count++;
        // if (count > 6)
        // {
        //     printf("%s too long\n", (char*)&w);
        //     bye(0);
        // }
        CONTEXT_PTR->Env.get();
        if (strchr(Delim, CONTEXT_PTR->Chr))
        {
            break;
        }
        if (CONTEXT_PTR->Chr == '\\')
        {
            CONTEXT_PTR->Env.get();
        }
        putByte(CONTEXT_PTR->Chr, &i, &w, &p, &c1);
    }

    y = popSym(i, w, p, &c1);
    //printf("%p --> CAR = %p CDR = %p \n", y, y->car, y->cdr);
    if (x = symToNum(tail(y), 0, '.', 0))
    {
        return x;
    }
    if (x = isIntern(tail(y), CONTEXT_PTR->Intern))
    {
        return x;
    }

    intern(y, CONTEXT_PTR->Intern);
    //val(y) = Nil;
    return y;
}

any read1(int end)
{
   if (!CONTEXT_PTR->Chr)
      CONTEXT_PTR->Env.get();
   if (CONTEXT_PTR->Chr == end)
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

    if (!CONTEXT_PTR->Chr)
        CONTEXT_PTR->Env.get();
    if (skipc(c) < 0)
        return Nil;
    if (CONTEXT_PTR->Chr == '"')
    {
        CONTEXT_PTR->Env.get();
        if (CONTEXT_PTR->Chr == '"')
        {
            CONTEXT_PTR->Env.get();
            return Nil;
        }
        if (!testEsc())
            return Nil;
        Push(c1, y =  cons(mkChar(CONTEXT_PTR->Chr), Nil));
        while (CONTEXT_PTR->Env.get(), CONTEXT_PTR->Chr != '"' && testEsc())
            y = cdr(y) = cons(mkChar(CONTEXT_PTR->Chr), Nil);
        CONTEXT_PTR->Env.get();
        return Pop(c1);
    }
    if (CONTEXT_PTR->Chr >= '0' && CONTEXT_PTR->Chr <= '9')
    {
        putByte1(CONTEXT_PTR->Chr, &i, &w, &p);
        while (CONTEXT_PTR->Env.get(), CONTEXT_PTR->Chr >= '0' && CONTEXT_PTR->Chr <= '9' || CONTEXT_PTR->Chr == '.')
            putByte(CONTEXT_PTR->Chr, &i, &w, &p, &c1);
        return symToNum(tail(popSym(i, w, p, &c1)), 0, '.', 0);
    }
    if (CONTEXT_PTR->Chr != '+' && CONTEXT_PTR->Chr != '-')
    {
        // TODO check what needs to be done about stack - FREE MUST BE ADDED
        // char nm[bufSize(x)];
        char *nm = (char *)malloc(bufSize(x));

        bufString(x, nm);
        if (CONTEXT_PTR->Chr >= 'A' && CONTEXT_PTR->Chr <= 'Z' || CONTEXT_PTR->Chr == '\\' || CONTEXT_PTR->Chr >= 'a' && CONTEXT_PTR->Chr <= 'z' || strchr(nm,CONTEXT_PTR->Chr))
        {
            if (CONTEXT_PTR->Chr == '\\')
                CONTEXT_PTR->Env.get();
            putByte1(CONTEXT_PTR->Chr, &i, &w, &p);
            while (CONTEXT_PTR->Env.get(),
                    CONTEXT_PTR->Chr >= '0' && CONTEXT_PTR->Chr <= '9' || CONTEXT_PTR->Chr >= 'A' && CONTEXT_PTR->Chr <= 'Z' ||
                    CONTEXT_PTR->Chr == '\\' || CONTEXT_PTR->Chr >= 'a' && CONTEXT_PTR->Chr <= 'z' || strchr(nm,CONTEXT_PTR->Chr) )
            {
                if (CONTEXT_PTR->Chr == '\\')
                    CONTEXT_PTR->Env.get();
                putByte(CONTEXT_PTR->Chr, &i, &w, &p, &c1);
            }
            y = popSym(i, w, p, &c1);
            if (x = isIntern(tail(y), CONTEXT_PTR->Intern))
            {
                free(nm);
                return x;
            }
            intern(y, CONTEXT_PTR->Intern);
            val(y) = Nil;
            free(nm);
            return y;
        }
    }
    y = mkTxt(c = CONTEXT_PTR->Chr);
    CONTEXT_PTR->Env.get();
    return mkChar(c);
}

bool eol(void)
{
   if (CONTEXT_PTR->Chr < 0)
      return YES;
   if (CONTEXT_PTR->Chr == '\n')
   {
      CONTEXT_PTR->Chr = 0;
      return YES;
   }
   if (CONTEXT_PTR->Chr == '\r')
   {
      CONTEXT_PTR->Env.get();
      if (CONTEXT_PTR->Chr == '\n')
         CONTEXT_PTR->Chr = 0;
      return YES;
   }
   return NO;
}
