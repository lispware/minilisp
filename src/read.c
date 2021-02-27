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
    if (!_CONTEXT_PTR->Home)
    {
        return symBytes(x);
    }
    return strlen(_CONTEXT_PTR->Home) + symBytes(x);
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
static any rdList(void)
{
    any x;
    cell c1;

    for (;;)
    {
        if (skip() == ')')
        {
            _CONTEXT_PTR->Env.get();
            return Nil;
        }
        if (_CONTEXT_PTR->Chr == ']')
        {
            return Nil;
        }
        if (_CONTEXT_PTR->Chr != '~')
        {
            x = cons(_CONTEXT_PTR, read0(NO),Nil);
            Push(c1, x);
            break;
        }
        _CONTEXT_PTR->Env.get();

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
            _CONTEXT_PTR->Env.get();
            break;
        }
        if (_CONTEXT_PTR->Chr == ']')
            break;
        if (_CONTEXT_PTR->Chr == '.')
        {
            _CONTEXT_PTR->Env.get();
            cdr(x) = skip()==')' || _CONTEXT_PTR->Chr==']'? data(c1) : read0(NO);
            if (skip() == ')')
                _CONTEXT_PTR->Env.get();
            else if (_CONTEXT_PTR->Chr != ']')
                err(NULL, x, "Bad dotted pair");
            break;
        }
        if (_CONTEXT_PTR->Chr != '~')
        {
            x = cdr(x) = cons(_CONTEXT_PTR, read0(NO),Nil);
        }
        else
        {
            _CONTEXT_PTR->Env.get();
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
    if (_CONTEXT_PTR->Chr == '(')
    {
        _CONTEXT_PTR->Env.get();
        x = rdList();
        if (top  &&  _CONTEXT_PTR->Chr == ']')
            _CONTEXT_PTR->Env.get();
        return x;
    }
    if (_CONTEXT_PTR->Chr == '[')
    {
        _CONTEXT_PTR->Env.get();
        x = rdList();
        if (_CONTEXT_PTR->Chr != ']')
            err(NULL, x, "Super parentheses mismatch");
        _CONTEXT_PTR->Env.get();
        return x;
    }
    if (_CONTEXT_PTR->Chr == '\'')
    {
        _CONTEXT_PTR->Env.get();
        return cons(_CONTEXT_PTR, doQuote_D, read0(top));
    }
    if (_CONTEXT_PTR->Chr == ',')
    {
        _CONTEXT_PTR->Env.get();
        return read0(top);
    }
    if (_CONTEXT_PTR->Chr == '`')
    {
        _CONTEXT_PTR->Env.get();
        Push(c1, read0(top));
        x = EVAL(data(c1));
        drop(c1);
        return x;
    }
    if (_CONTEXT_PTR->Chr == '"')
    {
        _CONTEXT_PTR->Env.get();
        if (_CONTEXT_PTR->Chr == '"')
        {
            _CONTEXT_PTR->Env.get();
            return Nil;
        }
        if (!testEsc())
            eofErr();
        putByte1(_CONTEXT_PTR->Chr, &i, &w, &p);
        while (_CONTEXT_PTR->Env.get(), _CONTEXT_PTR->Chr != '"')
        {
            if (!testEsc())
                eofErr();
            putByte(_CONTEXT_PTR->Chr, &i, &w, &p, &c1);
        }
        y = popSym(i, w, p, &c1),  _CONTEXT_PTR->Env.get();
        if (x = isIntern(tail(y), _CONTEXT_PTR->Transient))
            return x;
        intern(y, _CONTEXT_PTR->Transient);
        return y;
    }
    if (strchr(Delim, _CONTEXT_PTR->Chr))
        err(NULL, NULL, "Bad input '%c' (%d)", isprint(_CONTEXT_PTR->Chr)? _CONTEXT_PTR->Chr:'?', _CONTEXT_PTR->Chr);
    if (_CONTEXT_PTR->Chr == '\\')
        _CONTEXT_PTR->Env.get();
    putByte1(_CONTEXT_PTR->Chr, &i, &w, &p);

    int count=0;
    for (;;)
    {
        count++;
        // if (count > 6)
        // {
        //     printf("%s too long\n", (char*)&w);
        //     bye(0);
        // }
        _CONTEXT_PTR->Env.get();
        if (strchr(Delim, _CONTEXT_PTR->Chr))
        {
            break;
        }
        if (_CONTEXT_PTR->Chr == '\\')
        {
            _CONTEXT_PTR->Env.get();
        }
        putByte(_CONTEXT_PTR->Chr, &i, &w, &p, &c1);
    }

    y = popSym(i, w, p, &c1);
    //printf("%p --> CAR = %p CDR = %p \n", y, y->car, y->cdr);
    if (x = symToNum(_CONTEXT_PTR, tail(y), 0, '.', 0))
    {
        return x;
    }
    if (x = isIntern(tail(y), _CONTEXT_PTR->Intern))
    {
        return x;
    }

    intern(y, _CONTEXT_PTR->Intern);
    //val(y) = Nil;
    return y;
}

any read1(int end)
{
   if (!_CONTEXT_PTR->Chr)
      _CONTEXT_PTR->Env.get();
   if (_CONTEXT_PTR->Chr == end)
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

    if (!_CONTEXT_PTR->Chr)
        _CONTEXT_PTR->Env.get();
    if (skipc(c) < 0)
        return Nil;
    if (_CONTEXT_PTR->Chr == '"')
    {
        _CONTEXT_PTR->Env.get();
        if (_CONTEXT_PTR->Chr == '"')
        {
            _CONTEXT_PTR->Env.get();
            return Nil;
        }
        if (!testEsc())
            return Nil;
        Push(c1, y =  cons(_CONTEXT_PTR, mkChar(_CONTEXT_PTR->Chr), Nil));
        while (_CONTEXT_PTR->Env.get(), _CONTEXT_PTR->Chr != '"' && testEsc())
            y = cdr(y) = cons(_CONTEXT_PTR, mkChar(_CONTEXT_PTR->Chr), Nil);
        _CONTEXT_PTR->Env.get();
        return Pop(c1);
    }
    if (_CONTEXT_PTR->Chr >= '0' && _CONTEXT_PTR->Chr <= '9')
    {
        putByte1(_CONTEXT_PTR->Chr, &i, &w, &p);
        while (_CONTEXT_PTR->Env.get(), _CONTEXT_PTR->Chr >= '0' && _CONTEXT_PTR->Chr <= '9' || _CONTEXT_PTR->Chr == '.')
            putByte(_CONTEXT_PTR->Chr, &i, &w, &p, &c1);
        return symToNum(_CONTEXT_PTR, tail(popSym(i, w, p, &c1)), 0, '.', 0);
    }
    if (_CONTEXT_PTR->Chr != '+' && _CONTEXT_PTR->Chr != '-')
    {
        // TODO check what needs to be done about stack - FREE MUST BE ADDED
        // char nm[bufSize(x)];
        char *nm = (char *)malloc(bufSize(x));

        bufString(x, nm);
        if (_CONTEXT_PTR->Chr >= 'A' && _CONTEXT_PTR->Chr <= 'Z' || _CONTEXT_PTR->Chr == '\\' || _CONTEXT_PTR->Chr >= 'a' && _CONTEXT_PTR->Chr <= 'z' || strchr(nm,_CONTEXT_PTR->Chr))
        {
            if (_CONTEXT_PTR->Chr == '\\')
                _CONTEXT_PTR->Env.get();
            putByte1(_CONTEXT_PTR->Chr, &i, &w, &p);
            while (_CONTEXT_PTR->Env.get(),
                    _CONTEXT_PTR->Chr >= '0' && _CONTEXT_PTR->Chr <= '9' || _CONTEXT_PTR->Chr >= 'A' && _CONTEXT_PTR->Chr <= 'Z' ||
                    _CONTEXT_PTR->Chr == '\\' || _CONTEXT_PTR->Chr >= 'a' && _CONTEXT_PTR->Chr <= 'z' || strchr(nm,_CONTEXT_PTR->Chr) )
            {
                if (_CONTEXT_PTR->Chr == '\\')
                    _CONTEXT_PTR->Env.get();
                putByte(_CONTEXT_PTR->Chr, &i, &w, &p, &c1);
            }
            y = popSym(i, w, p, &c1);
            if (x = isIntern(tail(y), _CONTEXT_PTR->Intern))
            {
                free(nm);
                return x;
            }
            intern(y, _CONTEXT_PTR->Intern);
            val(y) = Nil;
            free(nm);
            return y;
        }
    }
    y = mkTxt(c = _CONTEXT_PTR->Chr);
    _CONTEXT_PTR->Env.get();
    return mkChar(c);
}

bool eol(void)
{
   if (_CONTEXT_PTR->Chr < 0)
      return YES;
   if (_CONTEXT_PTR->Chr == '\n')
   {
      _CONTEXT_PTR->Chr = 0;
      return YES;
   }
   if (_CONTEXT_PTR->Chr == '\r')
   {
      _CONTEXT_PTR->Env.get();
      if (_CONTEXT_PTR->Chr == '\n')
         _CONTEXT_PTR->Chr = 0;
      return YES;
   }
   return NO;
}
