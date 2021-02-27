#include "lisp.h"
#include "cell.h"


#if INTPTR_MAX == INT32_MAX
    #include "def32.d"
#elif INTPTR_MAX == INT64_MAX
    #include "def64.d"
#else
    #error "Unsupported bit width"
#endif


static any read0(Context *, bool);

static char Delim[] = " \t\n\r\"'(),[]`~{}";

/* Buffer size */
int bufSize(any x)
{
    return symBytes(x) + 1;
}

int pathSize(Context *CONTEXT_PTR, any x)
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
    _CONTEXT_PTR->Chr = getc(_CONTEXT_PTR->InFile);
}


/* Skip White Space and Comments */
static int skipc(int c)
{
    if (_CONTEXT_PTR->Chr < 0)
    {
        return _CONTEXT_PTR->Chr;
    }
    for (;;)
    {
        while (_CONTEXT_PTR->Chr <= ' ')
        {
            _CONTEXT_PTR->Env.get();
            if (_CONTEXT_PTR->Chr < 0)
            {
                return _CONTEXT_PTR->Chr;
            }
        }
        if (_CONTEXT_PTR->Chr != c)
        {
            return _CONTEXT_PTR->Chr;
        }
        _CONTEXT_PTR->Env.get();
        while (_CONTEXT_PTR->Chr != '\n')
        {
            if (_CONTEXT_PTR->Chr < 0)
            {
                return _CONTEXT_PTR->Chr;
            }
            _CONTEXT_PTR->Env.get();
        }
    }
}

void comment(void)
{
    _CONTEXT_PTR->Env.get();
    if (_CONTEXT_PTR->Chr != '{')
    {
        while (_CONTEXT_PTR->Chr != '\n')
        {
            if (_CONTEXT_PTR->Chr < 0)
            {
                return;
            }
            _CONTEXT_PTR->Env.get();
        }
    }
    else
    {
        int n = 0;

        for (;;) {  // #{block-comment}# from Kriangkrai Soatthiyanont
            _CONTEXT_PTR->Env.get();
            if (_CONTEXT_PTR->Chr < 0)
            {
                return;
            }
            if (_CONTEXT_PTR->Chr == '#'  &&  (_CONTEXT_PTR->Env.get(), _CONTEXT_PTR->Chr == '{'))
            {
                ++n;
            }
            else if (_CONTEXT_PTR->Chr == '}'  &&  (_CONTEXT_PTR->Env.get(), _CONTEXT_PTR->Chr == '#')  &&  --n < 0)
            {
                break;
            }
        }
        _CONTEXT_PTR->Env.get();
    }
}

static int skip(void)
{
    for (;;)
    {
        if (_CONTEXT_PTR->Chr < 0)
        {
            return _CONTEXT_PTR->Chr;
        }
        while (_CONTEXT_PTR->Chr <= ' ')
        {
            _CONTEXT_PTR->Env.get();
            if (_CONTEXT_PTR->Chr < 0)
            {
                return _CONTEXT_PTR->Chr;
            }
        }

        if (_CONTEXT_PTR->Chr != '#')
        {
            return _CONTEXT_PTR->Chr;
        }
        comment();
    }
}

/* Test for escaped characters */
static bool testEsc(void)
{
    for (;;)
    {
        if (_CONTEXT_PTR->Chr < 0)
            return NO;
        if (_CONTEXT_PTR->Chr != '\\')
            return YES;
        if (_CONTEXT_PTR->Env.get(), _CONTEXT_PTR->Chr != '\n')
            return YES;
        do
        {
            _CONTEXT_PTR->Env.get();
        }
        while (_CONTEXT_PTR->Chr == ' '  ||  _CONTEXT_PTR->Chr == '\t');
    }
}

/* Read a list */
static any rdList(Context *CONTEXT_PTR)
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
            x = cons(CONTEXT_PTR, read0(CONTEXT_PTR, NO),Nil);
            Push(c1, x);
            break;
        }
        CONTEXT_PTR->Env.get();

        x = read0(CONTEXT_PTR, NO);
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
            cdr(x) = skip()==')' || CONTEXT_PTR->Chr==']'? data(c1) : read0(CONTEXT_PTR, NO);
            if (skip() == ')')
                CONTEXT_PTR->Env.get();
            else if (CONTEXT_PTR->Chr != ']')
                err(NULL, x, "Bad dotted pair");
            break;
        }
        if (CONTEXT_PTR->Chr != '~')
        {
            x = cdr(x) = cons(CONTEXT_PTR, read0(CONTEXT_PTR, NO),Nil);
        }
        else
        {
            CONTEXT_PTR->Env.get();
            cdr(x) = read0(CONTEXT_PTR, NO);
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
static any read0(Context *CONTEXT_PTR, bool top)
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
        x = rdList(CONTEXT_PTR);
        if (top  &&  CONTEXT_PTR->Chr == ']')
            CONTEXT_PTR->Env.get();
        return x;
    }
    if (CONTEXT_PTR->Chr == '[')
    {
        CONTEXT_PTR->Env.get();
        x = rdList(CONTEXT_PTR);
        if (CONTEXT_PTR->Chr != ']')
            err(NULL, x, "Super parentheses mismatch");
        CONTEXT_PTR->Env.get();
        return x;
    }
    if (CONTEXT_PTR->Chr == '\'')
    {
        CONTEXT_PTR->Env.get();
        return cons(CONTEXT_PTR, doQuote_D, read0(CONTEXT_PTR, top));
    }
    if (CONTEXT_PTR->Chr == ',')
    {
        CONTEXT_PTR->Env.get();
        return read0(CONTEXT_PTR, top);
    }
    if (CONTEXT_PTR->Chr == '`')
    {
        CONTEXT_PTR->Env.get();
        Push(c1, read0(CONTEXT_PTR, top));
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
    if (x = symToNum(CONTEXT_PTR, tail(y), 0, '.', 0))
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

any read1(Context *CONTEXT_PTR, int end)
{
   if (!CONTEXT_PTR->Chr)
      CONTEXT_PTR->Env.get();
   if (CONTEXT_PTR->Chr == end)
      return Nil;
   return read0(CONTEXT_PTR, YES);
}


bool eol(Context *CONTEXT_PTR)
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
