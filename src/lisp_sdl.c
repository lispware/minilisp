#include "pico.h"
#include <SDL2/SDL.h>

#define PACK(__M, __R) any __R; { \
word __r = (word)__M; \
word allOnes = -1; \
uword __mask = ((uword)allOnes >> (BITS / 2)); \
word __low = __r & __mask; \
word __high = (__r >> (BITS / 2))  & __mask; \
__R = cons(box(__high), box(__low)); }

#define UNPACK(__M, __R) uword __R; {\
	uword __H = unBox(car(__M));\
	uword __L = unBox(cdr(__M));\
	__R = (__H << (BITS / 2)) | __L; }



any LISP_SDL_CreateWindow(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));
	x = cdr(x);
	any p2 = EVAL(car(x));
	x = cdr(x);
	any p3 = EVAL(car(x));

	char *windowTitle = (char *)calloc(bufSize(p1), 1);
	bufString(p1, windowTitle);

	word W = unBox(p2);
	word H = unBox(p3);

	SDL_Window *window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, 0);
	PACK(window, P);
	return P;
}

any LISP_SDL_CreateRenderer(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));

    UNPACK(p1, w);
    SDL_Window *window = (SDL_Window*)w;
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    PACK(renderer, P);
    return P;
}
