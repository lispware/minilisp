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
int bufSize(Context *CONTEXT_PTR, any x)
{
    return symBytes(CONTEXT_PTR, x) + 1;
}

int pathSize(Context *CONTEXT_PTR, any x)
{
    int c = firstByte(CONTEXT_PTR, x);

    if (c != '@'  &&  (c != '+'))
    {
        return bufSize(CONTEXT_PTR, x);
    }
    if (!CONTEXT_PTR->Home)
    {
        return symBytes(CONTEXT_PTR, x);
    }
    return strlen(CONTEXT_PTR->Home) + symBytes(CONTEXT_PTR, x);
}

// void bufString(Context *CONTEXT_PTR, any x, char *p)
// {
//     int c, i;
//     uword w;
// 
//     if (!isNil(x))
//     {
//         for (x = name(x), c = getByte1(CONTEXT_PTR, &i, &w, &x); c; c = getByte(CONTEXT_PTR, &i, &w, &x))
//         {
//             if (c == '^')
//             {
//                 if ((c = getByte(CONTEXT_PTR, &i, &w, &x)) == '?')
//                 {
//                     c = 127;
//                 }
//                 else
//                 {
//                     c &= 0x1F;
//                 }
//             }
//             *p++ = c;
//         }
//     }
//     *p = '\0';
// }


/*** Reading ***/
void getStdin(Context *CONTEXT_PTR)
{
    CONTEXT_PTR->Chr = getc(CONTEXT_PTR->InFile);
}


void comment(Context *CONTEXT_PTR)
{
    CONTEXT_PTR->Env.get(CONTEXT_PTR);
    if (CONTEXT_PTR->Chr != '{')
    {
        while (CONTEXT_PTR->Chr != '\n')
        {
            if (CONTEXT_PTR->Chr < 0)
            {
                return;
            }
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
        }
    }
    else
    {
        int n = 0;

        for (;;) {  // #{block-comment}# from Kriangkrai Soatthiyanont
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
            if (CONTEXT_PTR->Chr < 0)
            {
                return;
            }
            if (CONTEXT_PTR->Chr == '#'  &&  (CONTEXT_PTR->Env.get(CONTEXT_PTR), CONTEXT_PTR->Chr == '{'))
            {
                ++n;
            }
            else if (CONTEXT_PTR->Chr == '}'  &&  (CONTEXT_PTR->Env.get(CONTEXT_PTR), CONTEXT_PTR->Chr == '#')  &&  --n < 0)
            {
                break;
            }
        }
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
    }
}

static int skip(Context *CONTEXT_PTR)
{
    for (;;)
    {
        if (CONTEXT_PTR->Chr < 0)
        {
            return CONTEXT_PTR->Chr;
        }
        while (CONTEXT_PTR->Chr <= ' ')
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
            if (CONTEXT_PTR->Chr < 0)
            {
                return CONTEXT_PTR->Chr;
            }
        }

        if (CONTEXT_PTR->Chr != '#')
        {
            return CONTEXT_PTR->Chr;
        }
        comment(CONTEXT_PTR);
    }
}

/* Test for escaped characters */
static bool testEsc(Context *CONTEXT_PTR)
{
    for (;;)
    {
        if (CONTEXT_PTR->Chr < 0)
            return NO;
        if (CONTEXT_PTR->Chr != '\\')
            return YES;
        if (CONTEXT_PTR->Env.get(CONTEXT_PTR), CONTEXT_PTR->Chr != '\n')
            return YES;
        do
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
        }
        while (CONTEXT_PTR->Chr == ' '  ||  CONTEXT_PTR->Chr == '\t');
    }
}

/* Read a list */
static any rdList(Context *CONTEXT_PTR)
{
    any x;
    cell c1;

    for (;;)
    {
        if (skip(CONTEXT_PTR) == ')')
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
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
        CONTEXT_PTR->Env.get(CONTEXT_PTR);

        x = read0(CONTEXT_PTR, NO);
        Push(c1, x);
        if (isCell(x = data(c1) = EVAL(CONTEXT_PTR, data(c1))))
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
        if (skip(CONTEXT_PTR) == ')')
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
            break;
        }
        if (CONTEXT_PTR->Chr == ']')
            break;
        if (CONTEXT_PTR->Chr == '.')
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
            cdr(x) = skip(CONTEXT_PTR)==')' || CONTEXT_PTR->Chr==']'? data(c1) : read0(CONTEXT_PTR, NO);
            if (skip(CONTEXT_PTR) == ')')
                CONTEXT_PTR->Env.get(CONTEXT_PTR);
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
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
            cdr(x) = read0(CONTEXT_PTR, NO);
            cdr(x) = EVAL(CONTEXT_PTR, cdr(x));
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

    if (skip(CONTEXT_PTR) < 0)
    {
        if (top)
            return Nil;
        eofErr();
    }
    if (CONTEXT_PTR->Chr == '(')
    {
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        x = rdList(CONTEXT_PTR);
        if (top  &&  CONTEXT_PTR->Chr == ']')
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
        return x;
    }
    if (CONTEXT_PTR->Chr == '[')
    {
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        x = rdList(CONTEXT_PTR);
        if (CONTEXT_PTR->Chr != ']')
            err(NULL, x, "Super parentheses mismatch");
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        return x;
    }
    if (CONTEXT_PTR->Chr == '\'')
    {
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        return cons(CONTEXT_PTR, doQuote_D, read0(CONTEXT_PTR, top));
    }
    if (CONTEXT_PTR->Chr == ',')
    {
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        return read0(CONTEXT_PTR, top);
    }
    if (CONTEXT_PTR->Chr == '`')
    {
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        Push(c1, read0(CONTEXT_PTR, top));
        x = EVAL(CONTEXT_PTR, data(c1));
        drop(c1);
        return x;
    }
    if (CONTEXT_PTR->Chr == '"')
    {
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        if (CONTEXT_PTR->Chr == '"')
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
            return Nil;
        }
        if (!testEsc(CONTEXT_PTR))
            eofErr();
        putByte1(CONTEXT_PTR->Chr, &i, &w, &p);
        while (CONTEXT_PTR->Env.get(CONTEXT_PTR), CONTEXT_PTR->Chr != '"')
        {
            if (!testEsc(CONTEXT_PTR))
                eofErr();
            putByte(CONTEXT_PTR, CONTEXT_PTR->Chr, &i, &w, &p, &c1);
        }
        y = popSym(CONTEXT_PTR, i, w, p, &c1),  CONTEXT_PTR->Env.get(CONTEXT_PTR);
        if (x = isIntern(CONTEXT_PTR, tail(y), CONTEXT_PTR->Transient))
            return x;
        intern(CONTEXT_PTR, y, CONTEXT_PTR->Transient);
        return y;
    }
    if (strchr(Delim, CONTEXT_PTR->Chr))
        err(NULL, NULL, "Bad input '%c' (%d)", isprint(CONTEXT_PTR->Chr)? CONTEXT_PTR->Chr:'?', CONTEXT_PTR->Chr);
    if (CONTEXT_PTR->Chr == '\\')
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
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
        CONTEXT_PTR->Env.get(CONTEXT_PTR);
        if (strchr(Delim, CONTEXT_PTR->Chr))
        {
            break;
        }
        if (CONTEXT_PTR->Chr == '\\')
        {
            CONTEXT_PTR->Env.get(CONTEXT_PTR);
        }
        putByte(CONTEXT_PTR, CONTEXT_PTR->Chr, &i, &w, &p, &c1);
    }

    y = popSym(CONTEXT_PTR, i, w, p, &c1);
    //printf("%p --> CAR = %p CDR = %p \n", y, y->car, y->cdr);
    if (x = symToNum(CONTEXT_PTR, tail(y), 0, '.', 0))
    {
        return x;
    }
    if (x = isIntern(CONTEXT_PTR, tail(y), CONTEXT_PTR->Intern))
    {
        return x;
    }

    intern(CONTEXT_PTR, y, CONTEXT_PTR->Intern);
    //val(y) = Nil;
    return y;
}

any read1(Context *CONTEXT_PTR, int end)
{
   if (!CONTEXT_PTR->Chr)
      CONTEXT_PTR->Env.get(CONTEXT_PTR);
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
      CONTEXT_PTR->Env.get(CONTEXT_PTR);
      if (CONTEXT_PTR->Chr == '\n')
         CONTEXT_PTR->Chr = 0;
      return YES;
   }
   return NO;
}
