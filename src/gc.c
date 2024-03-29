/* 26oct14abu
 * (c) Software Lab. Alexander Burger
 */

#include "pico.h"

static void mark(any);

/* Mark data */
static void markTail(any x) {
   while (isCell(x)) {
      if (!(num(cdr(x)) & 1))
         return;
      *(word*)&cdr(x) &= ~1;
      mark(cdr(x)),  x = car(x);
   }
   if (!isTxt(x))
      do {
         if (!(num(val(x)) & 1))
            return;
         *(word*)&val(x) &= ~1;
      } while (!isNum(x = val(x)));
}

static void mark(any x) {
   while (isCell(x)) {
      if (!(num(cdr(x)) & 1))
         return;
      *(word*)&cdr(x) &= ~1;
      mark(car(x)),  x = cdr(x);
   }
   if (!isNum(x)  &&  num(val(x)) & 1) {
      *(word*)&val(x) &= ~1;
      mark(val(x));
      markTail(tail(x));
   }
}

/* Garbage collector */
static void gc(word c) {
   any p;
   heap *h;
   int i;

   h = Heaps;
   do {
      p = h->cells + CELLS-1;
      do
         *(word*)&cdr(p) |= 1;
      while (--p >= h->cells);
   } while (h = h->next);
   /* Mark */
   for (i = 0;  i < RAMS;  i += 2) {
      markTail(Ram[i]);
      mark(Ram[i+1]);
   }
   mark(Intern[0]),  mark(Intern[1]);
   mark(Transient[0]), mark(Transient[1]);
   mark(ApplyArgs),  mark(ApplyBody);
   for (p = Env.stack; p; p = cdr(p))
      mark(car(p));
   for (p = (any)Env.bind;  p;  p = (any)((bindFrame*)p)->link)
      for (i = ((bindFrame*)p)->cnt;  --i >= 0;) {
         mark(((bindFrame*)p)->bnd[i].sym);
         mark(((bindFrame*)p)->bnd[i].val);
      }
   for (p = (any)CatchPtr; p; p = (any)((catchFrame*)p)->link) {
      if (((catchFrame*)p)->tag)
         mark(((catchFrame*)p)->tag);
      mark(((catchFrame*)p)->fin);
   }
   /* Sweep */
   Avail = NULL;
   h = Heaps;
   if (c) {
      do {
         p = h->cells + CELLS-1;
         do
            if (num(p->cdr) & 1)
               Free(p),  --c;
         while (--p >= h->cells);
      } while (h = h->next);
      while (c >= 0)
         heapAlloc(),  c -= CELLS;
   }
   else {
      heap **hp = &Heaps;
      cell *av;

      do {
         c = CELLS;
         av = Avail;
         p = h->cells + CELLS-1;
         do
            if (num(p->cdr) & 1)
               Free(p),  --c;
         while (--p >= h->cells);
         if (c)
            hp = &h->next,  h = h->next;
         else
            Avail = av,  h = h->next,  free(*hp),  *hp = h;
      } while (h);
   }
}

// (gc ['num]) -> num | NIL
any doGc(any x) {
   x = cdr(x),  x = EVAL(car(x));
   val(At) = val(At2) = Nil;
   gc(isNum(x)? unBox(x) * 1024 / sizeof(cell) : CELLS);  // kB
   return x;
}

/* Construct a cell */
any cons(any x, any y) {
   cell *p;

   if (!(p = Avail)) {
      cell c1, c2;

      Push(c1,x);
      Push(c2,y);
      gc(CELLS);
      drop(c1);
      p = Avail;
   }
   Avail = p->car;
   p->car = x;
   p->cdr = y;
   return p;
}

/* Construct a symbol */
any consSym(any val, uword w) {
   cell *p;

   if (!(p = Avail)) {
      cell c1;

      if (!val)
         gc(CELLS);
      else {
         Push(c1,val);
         gc(CELLS);
         drop(c1);
      }
      p = Avail;
   }
   Avail = p->car;
   p = symPtr(p);
   val(p) = val ? val : p;
   tail(p) = txt(w);
   return p;
}

/* Construct a name cell */
any consName(uword w, any n) {
   cell *p;

   if (!(p = Avail)) {
      gc(CELLS);
      p = Avail;
   }
   Avail = p->car;
   p = symPtr(p);
   val(p) = n;
   tail(p) = (any)w;
   return p;
}
