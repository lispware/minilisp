#include <lisp.h>
#include <SDL.h>
#include <stdio.h>

#define SCREEN_SIZE 1000

#define GetNumberParam(p, r) if (isNum(p)) r = mp_get_i32(num((p))); else r = 0;

SDL_Texture *texture, *text;

void drawPixel(SDL_Surface *surface, int x, int y, SDL_Color color)
{
    // map color to screen color
    Uint32 screenColor = SDL_MapRGB(surface->format, color.r, color.g, color.b);
    // Calculate location of pixel
    char *pPixelAddress = (char *)surface->pixels
        + x * surface->format->BytesPerPixel
        + y *surface->pitch ;
    // check and the lock the surface
    if (SDL_MUSTLOCK(surface))
    {
        int retValue = SDL_LockSurface(surface);
        if (retValue == -1)
        {
            printf("Count not lock surface. %d", SDL_GetError());
            exit(1);
        }
    }
    // copy directly to memory
    memcpy(pPixelAddress, &screenColor, surface->format->BytesPerPixel);
    if (SDL_MUSTLOCK(surface))
    {
        SDL_UnlockSurface(surface);
    }
}

Context LISP_CONTEXT;
SDL_Event LISP_SDL_EVENT;
SDL_Window* LISP_SDL_WINDOW = NULL;
SDL_Surface* LISP_SDL_SURFACE = NULL;

any lispsdlGetEvent(Context *CONTEXT_PTR, any x)
{
    if (SDL_PollEvent( &LISP_SDL_EVENT ))
    {
        return T;
    }
    else
    {
        return Nil;
    }
}

any lispsdlIsCloseEvent(Context *CONTEXT_PTR, any x)
{
    if (LISP_SDL_EVENT.type == SDL_WINDOWEVENT && LISP_SDL_EVENT.window.event == SDL_WINDOWEVENT_CLOSE)
    {
        return T;
    }
    else
    {
        return Nil;
    }
}

any lispsdlIsTextInput(Context *CONTEXT_PTR, any x)
{
    if (LISP_SDL_EVENT.type == SDL_TEXTINPUT)
    {
        int x =  ((char *)LISP_SDL_EVENT.text.text)[0];
        mp_int *n = (mp_int*)malloc(sizeof(mp_int));
        mp_err _mp_error = mp_init(n); // TODO handle the errors appropriately
        mp_set(n, x);

        NewNumber(ext, n, r);

        return r;
    }
    else
    {
        return Nil;
    }
}

any lispsdlCloseWindow(Context *CONTEXT_PTR, any x)
{
    SDL_DestroyWindow(LISP_SDL_WINDOW);
    SDL_Quit();
    return Nil;
}

any lispsdlIsEnterPressed(Context *CONTEXT_PTR, any x)
{
    if (LISP_SDL_EVENT.type == SDL_KEYDOWN && LISP_SDL_EVENT.key.keysym.sym == SDLK_RETURN)
    {
        return T;
    }
    else
    {
        return Nil;
    }
}

any lispsdlIsBackspacePressed(Context *CONTEXT_PTR, any x)
{
    if (LISP_SDL_EVENT.type == SDL_KEYDOWN && LISP_SDL_EVENT.key.keysym.sym == SDLK_BACKSPACE)
    {
        return T;
    }
    else
    {
        return Nil;
    }
}

any lispsdlPutPixel(Context *CONTEXT_PTR, any ex)
{
    cell c1, c2, c3, c4, c5;
    ex = cdr(ex);
    any p1 = EVAL(CONTEXT_PTR, car(ex));
    Push(c1, p1);
    ex = cdr(ex);
    any p2 = EVAL(CONTEXT_PTR, car(ex));
    Push(c2, p2);
    ex = cdr(ex);
    any p3 = EVAL(CONTEXT_PTR, car(ex));
    Push(c3, p3);
    ex = cdr(ex);
    any p4 = EVAL(CONTEXT_PTR, car(ex));
    Push(c4, p4);
    ex = cdr(ex);
    any p5 = EVAL(CONTEXT_PTR, car(ex));
    Push(c5, p5);

    int x, y, r, g, b;

    GetNumberParam(p1, x);
    GetNumberParam(p2, y);
    GetNumberParam(p3, r);
    GetNumberParam(p4, g);
    GetNumberParam(p5, b);
    drop(c1);

    SDL_Color color;
    color.r = r;
    color.g = g;
    color.b = b;

    LISP_SDL_SURFACE = SDL_GetWindowSurface(LISP_SDL_WINDOW);
    drawPixel (LISP_SDL_SURFACE, x, y, color);

    return Nil;
}

any lispsdlUpdateWindow(Context *CONTEXT_PTR, any ex)
{
    SDL_UpdateWindowSurface(LISP_SDL_WINDOW);
    return T;
}

any lispsdlCreateWindow(Context *CONTEXT_PTR, any ex)
{
    cell c1, c2, c3;
    ex = cdr(ex);
    any p1 = EVAL(CONTEXT_PTR, car(ex));
    Push(c1, p1);

    ex = cdr(ex);
    any p2 = EVAL(CONTEXT_PTR, car(ex));
    Push(c2, p2);

    ex = cdr(ex);
    any p3 = EVAL(CONTEXT_PTR, car(ex));
    Push(c3, p3);


    int ps = pathSize(CONTEXT_PTR, p1);
    char *nm = (char*)malloc(ps);
    pathString(CONTEXT_PTR, p1, nm);

    int x, y;
    GetNumberParam(p2, x);
    GetNumberParam(p3, y);
    drop(c1);

    LISP_SDL_WINDOW = SDL_CreateWindow
        (
         nm,
         SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
         x, y,
         SDL_WINDOW_SHOWN
        );

    if (LISP_SDL_WINDOW == NULL)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return 1;
    }

    LISP_SDL_SURFACE = SDL_GetWindowSurface(LISP_SDL_WINDOW);

    SDL_StartTextInput();
}

#undef main
int main(int argc, char* av[])
{
    Context *CONTEXT_PTR = &LISP_CONTEXT;
    setupBuiltinFunctions(&CONTEXT_PTR->Mem);
    addBuiltinFunction(&CONTEXT_PTR->Mem, "sdlCreateWindow", lispsdlCreateWindow);
    addBuiltinFunction(&CONTEXT_PTR->Mem, "sdlpoll", lispsdlGetEvent);
    addBuiltinFunction(&CONTEXT_PTR->Mem, "sdlisclose", lispsdlIsCloseEvent);
    addBuiltinFunction(&CONTEXT_PTR->Mem, "sdlclose", lispsdlCloseWindow);
    addBuiltinFunction(&CONTEXT_PTR->Mem, "sdlistextinput", lispsdlIsTextInput);
    addBuiltinFunction(&CONTEXT_PTR->Mem, "sdisenterpressed", lispsdlIsEnterPressed);
    addBuiltinFunction(&CONTEXT_PTR->Mem, "sdisbackspacepressed", lispsdlIsBackspacePressed);
    addBuiltinFunction(&CONTEXT_PTR->Mem, "sdlputpixel", lispsdlPutPixel);
    addBuiltinFunction(&CONTEXT_PTR->Mem, "sdlupdatewindow", lispsdlUpdateWindow);
    initialize_context(CONTEXT_PTR);
    av++;
    CONTEXT_PTR->AV = av;
    CONTEXT_PTR->InFile = stdin, CONTEXT_PTR->Env.get = getStdin;
    CONTEXT_PTR->OutFile = stdout, CONTEXT_PTR->Env.put = putStdout;
    CONTEXT_PTR->ApplyArgs = Nil;
    CONTEXT_PTR->ApplyBody = Nil;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return 1;
    }

    loadAll(CONTEXT_PTR, NULL);

    SDL_DestroyWindow(LISP_SDL_WINDOW);
    SDL_Quit();
    return 0;
}
