#include "lisp.h"
#include "cell.h"


#if INTPTR_MAX == INT32_MAX
    #include "def32.d"
#elif INTPTR_MAX == INT64_MAX
    #include "def64.d"
#else
    #error "Unsupported bit width"
#endif

int firstByte(Context*CONTEXT_PTR, any s)
{
    if (getCARType(s) == TXT)
    {
        return ((uword)(s->car)) & 127;
    }
    else if (getCARType(s) == BIN_START)
    {
        return ((uword)(s->car->car)) & 127;
    }
    else
    {
        giveup("Cant get first byte");
    }
}

int getByte1(Context *CONTEXT_PTR, int *i, uword *p, any *q)
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

int getByte(Context *CONTEXT_PTR, int *i, uword *p, any *q)
{
    int c;

    if (*i == 0)
    {
        if (!*q || *q == Nil)
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

any mkChar(Context *CONTEXT_PTR, int c)
{
   return consSym(CONTEXT_PTR, NULL, c & 127);
}

void putByte0(int *i, uword *p, any *q)
{
    *p = 0;
    *i = 0;
    *q = NULL;
}

void putByte1(int c, int *i, uword *p, any *q)
{
    *p = c & 127;
    *i = 8;
    *q = NULL;
}

void putByte(Context *CONTEXT_PTR, int c, int *i, uword *p, any *q, cell *cp)
{
    c = c & 127;
    int d = 8;

    if (*i != BITS)
        *p |= (uword)c << *i;

    if (*i + d  > BITS)
    {
        if (*q)
        {
            any x = consName(CONTEXT_PTR, *p, Zero);
            setCARType(x, BIN);
            (*q)->cdr = x;
            *q = x;
        }
        else
        {
            any x = consSym(CONTEXT_PTR, NULL, 0);
            setCARType(x, BIN_START);
            Push(*cp, x);
            any y = consName(CONTEXT_PTR, *p, Zero);
            setCARType(y, BIN);
            (*cp).car->car = *q = y;

        }
        *p = c >> BITS - *i;
        *i -= BITS;
    }

    *i += d;
}

any popSym(Context *CONTEXT_PTR, int i, uword n, any q, cell *cp)
{
    if (q)
    {
        //val(q) = i <= (BITS-2)? box(n) : consName(CONTEXT_PTR, n, Zero);
        q->cdr = consName(CONTEXT_PTR, n, Nil);
        return Pop(*cp);
    }
    return consSym(CONTEXT_PTR, NULL,n);
}

int symBytes(Context *CONTEXT_PTR, any x)
{
    int cnt = 0;
    uword w;

    if (isNil(x))
        return 0;

    CellPartType t = getCARType(x);

    if (t == TXT)
    {
        w = (uword)(x->car);
        while (w)
        {
            ++cnt;
            w >>= 8;
        }
    }
    else if (t == BIN_START)
    {

        x = x->car;
        while (x != Nil)
        {
			w = (uword)(x->car);
            while (w)
            {
                ++cnt;
                w >>= 8;
            }
            x = x->cdr;
        }
    }

    return cnt;
}

any isIntern(Context *CONTEXT_PTR, any nm, any tree[2])
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

any internBin(Context *CONTEXT_PTR, any sym, any tree[2])
{
    any nm, x, y, z;
    word n;

    x = tree[1];

    if (x == Nil)
    {
        tree[1] = consIntern(CONTEXT_PTR, sym, Nil);
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
            cdr(x) = n < 0 ? consIntern(CONTEXT_PTR, consIntern(CONTEXT_PTR, sym, Nil), Nil) : consIntern(CONTEXT_PTR, Nil, consIntern(CONTEXT_PTR, sym, Nil));
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
                cadr(x) = consIntern(CONTEXT_PTR, sym, Nil);
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
                cddr(x) = consIntern(CONTEXT_PTR, sym, Nil);
                return sym;
            }
        }
    }
}

any intern(Context *CONTEXT_PTR, any sym, any tree[2])
{
   any nm, x;
   word n;


   if (getCARType(sym) == BIN_START) return internBin(CONTEXT_PTR, sym, tree);

   nm = sym;

   x = tree[0];
   if (Nil == x)
   {
      tree[0] = consIntern(CONTEXT_PTR, sym, Nil);
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
         cdr(x) = n < 0 ? consIntern(CONTEXT_PTR, consIntern(CONTEXT_PTR, sym, Nil), Nil) : consIntern(CONTEXT_PTR, Nil, consIntern(CONTEXT_PTR, sym, Nil));
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
            cadr(x) = consIntern(CONTEXT_PTR, sym, Nil);
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
            cddr(x) = consIntern(CONTEXT_PTR, sym, Nil);
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
any mkSym(Context *CONTEXT_PTR, byte *s)
{
    int i;
    uword w;
    cell c1, *p;

    putByte1(*s++, &i, &w, &p);
    while (*s)
    {
        putByte(CONTEXT_PTR, *s++, &i, &w, &p, &c1);
    }
    return popSym(CONTEXT_PTR, i, w, p, &c1);
}

/* Make string */
any mkStr(Context *CONTEXT_PTR, char *s)
{
   if (s && *s)
   {
      return mkSym(CONTEXT_PTR, (byte *)s);
   }
   else
   {
      return Nil;
   }
}

// (==== ['sym ..]) -> NIL
any doHide(Context* CONTEXT_PTR, any ex)
{
   // TODO - is this needed?
   // printf("%p\n", ex);

   return Nil;
}

void makeError(any ex) {err(ex, NULL, "Not making");}

// (chop 'any) -> lst
any doChop(Context *CONTEXT_PTR, any x) {
    int c, i;
    uword w;
    char *h;
    cell c1, c2;
    any y = Nil;

    x = cdr(x);
    x = EVAL(CONTEXT_PTR, car(x));

    if(x==Nil) return Nil;

    c = getByte1(CONTEXT_PTR, &i, &w, &x);
    Push(c1, cons(CONTEXT_PTR, mkChar(CONTEXT_PTR, c), Nil));
    y = data(c1);
    while (c)
    {
        c = getByte(CONTEXT_PTR,&i, &w, &x);
        if (c)
        {
            y = cdr(y) = cons(CONTEXT_PTR, mkChar(CONTEXT_PTR, c), Nil);
        }
    }

    return Pop(c1);

}
