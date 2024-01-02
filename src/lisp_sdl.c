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

any LISP_SDL_PollEvent(any ex)
{
	SDL_Event event;
	if (SDL_PollEvent( &event ))
	{
		if (event.type == SDL_WINDOWEVENT)
		{
			any y;
			cell c1;

			PACK(event.type, eventType);
			Push(c1, y = cons(eventType, Nil));
			PACK(event.window.event, eventValue)
			y = cdr(y) = cons(eventValue, Nil);
			printf("WINDOW EVENT eventType = %p eventValue = %p\n", eventType, eventValue);
			return Pop(c1);
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			any y;
			cell c1;

			PACK(event.type, eventType);
			Push(c1, y = cons(eventType, Nil));
			PACK(event.window.event, eventValue)
			y = cdr(y) = cons(eventValue, Nil);
			printf("MOUSE EVENT eventType = %p eventValue = %p\n", eventType, eventValue);
			return Pop(c1);
		}

		return Nil;
	}
	else
	{
		return Nil;
	}
}


any LISP_SDL_DestroyWindow(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));

    UNPACK(p1, w);
    SDL_DestroyWindow((SDL_Window*)w);

    return Nil;
}

any LISP_SDL_Quit(any ex)
{
    SDL_Quit();
    return Nil;
}


any COMP_PACK(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));
	UNPACK(p1, word1);
	x = cdr(x);
	any p2 = EVAL(car(x));
	UNPACK(p2, word2);

	if (word1 == word2)
	{
		return T;
	}

	return Nil;
}

any doPACK(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));
	word n = unBox(p1);

	PACK(n, ret);
	return ret;
}

any LISP_SDL_Init(any ex)
{
	SDL_Init(SDL_INIT_VIDEO);
	return Nil;
}


any LISP_SDL_RenderDrawLine(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));
    UNPACK(p1, renderer);
    x = cdr(x);
	any X1 = EVAL(car(x));
    x = cdr(x);
	any Y1 = EVAL(car(x));
    x = cdr(x);
	any X2 = EVAL(car(x));
    x = cdr(x);
	any Y2 = EVAL(car(x));

	printf("%d %d %d %d\n", unBox(X1), unBox(Y1), unBox(X2), unBox(Y2));
	//SDL_SetRenderDrawColor((SDL_Renderer*)renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine((SDL_Renderer*)renderer, unBox(X1), unBox(Y1), unBox(X2), unBox(Y2));

	 //SDL_RenderPresent((SDL_Renderer*)renderer);

	return Nil;
}

any LISP_SDL_RenderPresent(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));

    UNPACK(p1, renderer);
    SDL_RenderPresent((SDL_Renderer*)renderer);

    return Nil;
}

any LISP_SDL_SetRenderDrawColor(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));

    UNPACK(p1, renderer);

    x = cdr(x);
	any R = EVAL(car(x));
    x = cdr(x);
	any G = EVAL(car(x));
    x = cdr(x);
	any B = EVAL(car(x));
	SDL_SetRenderDrawColor((SDL_Renderer*)renderer, R, G, B, SDL_ALPHA_OPAQUE);

    return Nil;
}

any LISP_SDL_GetMouseState(any ex)
{
	word mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	return cons(box(mouseX), box(mouseY));
}
