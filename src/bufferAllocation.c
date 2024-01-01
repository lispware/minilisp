#include "pico.h"

any doCalloc(any ex)
{
	any x, y;
	x = cdr(ex);
	y = EVAL(car(x));
	word n = isNil(y) ? 100 : unBox(y);

	void *mem = calloc(n, 1);
	word r = (word)mem;
	word allOnes = -1;
	uword mask = ((uword)allOnes >> (BITS / 2));
	word low = r & mask;
	word high = (r >> (BITS / 2))  & mask;

	return cons(box(high), box(low));
}

any doFree(any ex)
{
	any x, y;
	x = cdr(ex);
	y = EVAL(car(x));

	uword H = unBox(car(y));
	uword L = unBox(cdr(y));

	uword r = (H << (BITS / 2)) | L;;

	void *mem = (void*)r;

	free(mem);

	return Nil;
}
