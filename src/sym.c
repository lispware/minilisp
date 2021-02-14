#include "lisp.h"
#include "cell.h"


#if INTPTR_MAX == INT32_MAX
    #include "def32.d"
#elif INTPTR_MAX == INT64_MAX
    #include "def64.d"
#else
    #error "Unsupported bit width"
#endif

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

