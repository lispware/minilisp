#include <lisp.h>
#include <SDL2/SDL.h>
#include <stdio.h>

#define SCREEN_SIZE 1000

Uint32 timerFunc(Uint32 interval, void *param)
{
    SDL_Event event;
    SDL_UserEvent userevent;

    /* In this example, our callback pushes a function
    into the queue, and causes our callback to be called again at the
    same interval: */

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = param;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return(interval);
}

void drawPixel (SDL_Surface *surface, int x, int y, SDL_Color color) {
    // map color to screen color
    Uint32 screenColor = SDL_MapRGB(surface->format, color.r,
            color.g, color.b);
    // Calculate location of pixel
    char *pPixelAddress = (char *)surface->pixels
        + x * surface->format->BytesPerPixel
        + y *surface->pitch ;
    // check and the lock the surface
    if (SDL_MUSTLOCK(surface)) {
        int retValue = SDL_LockSurface(surface);
        if (retValue == -1) {
            printf("Count not lock surface. %d", SDL_GetError());
            exit(1);
        }
    }
    // copy directly to memory
    memcpy(pPixelAddress, &screenColor, surface->format->BytesPerPixel);
    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }
}

Context LISP_CONTEXT;

#if 0
#include "lisp.h"

Context LISP_CONTEXT;

int main(int argc, char *av[])
{
    Context *CONTEXT_PTR = &LISP_CONTEXT;
    setupBuiltinFunctions(&CONTEXT_PTR->Mem);
    initialize_context(CONTEXT_PTR);
    av++;
    CONTEXT_PTR->AV = av;

    CONTEXT_PTR->InFile = stdin, CONTEXT_PTR->Env.get = getStdin;
    CONTEXT_PTR->OutFile = stdout, CONTEXT_PTR->Env.put = putStdout;
    CONTEXT_PTR->ApplyArgs = Nil;
    CONTEXT_PTR->ApplyBody = Nil;

    loadAll(CONTEXT_PTR, NULL);
    while (!feof(stdin))
        load(CONTEXT_PTR, NULL, ':', Nil);

    return 0;
}
#endif

int main(int argc, char* av[])
{

    int i=0;
    uword w;
    any x, y;
    cell c1, *p;

    Context *CONTEXT_PTR = &LISP_CONTEXT;
    setupBuiltinFunctions(&CONTEXT_PTR->Mem);
    initialize_context(CONTEXT_PTR);
    av++;
    CONTEXT_PTR->AV = av;

    CONTEXT_PTR->InFile = stdin, CONTEXT_PTR->Env.get = getStdin;
    CONTEXT_PTR->OutFile = stdout, CONTEXT_PTR->Env.put = putStdout;
    CONTEXT_PTR->ApplyArgs = Nil;
    CONTEXT_PTR->ApplyBody = Nil;

    SDL_Window* window = NULL;
    SDL_Surface* surface = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow
        (
            "Graphics Demo",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_SIZE, SCREEN_SIZE,
            SDL_WINDOW_SHOWN
        );

    if (window == NULL)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return 1;
    }

    //surface = SDL_GetWindowSurface(window);
    //SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0xFF, 0x00, 0x00, 0x00));
    
    Uint32 delay = (33 / 10) * 10;  /* To round it down to the nearest 10 ms */
    SDL_TimerID my_timer_id = SDL_AddTimer(delay, timerFunc, NULL);



    char *xx;
    

    //SDL_StartTextInput();
    SDL_Event event;
    for(;;)
    {
        while( SDL_PollEvent( &event ) )
        {
            switch( event.type )
            {
                case SDL_WINDOWEVENT:
                    switch (event.window.event)
                    {
                        case SDL_WINDOWEVENT_CLOSE:
                            SDL_DestroyWindow(window);
                            SDL_Quit();
                            return 0;
                        default:
                            break;
                    }
                    break;
                case SDL_USEREVENT:
                    SDL_UpdateWindowSurface(window);
                    //RENDERER = loadRenderer();
                    break;

                case SDL_TEXTINPUT:
                    xx = event.text.text;
                    CONTEXT_PTR->Chr = xx[0];
                    if (i == 0)
                    {
                        putByte1(CONTEXT_PTR->Chr, &i, &w, &p);
                    }
                    else
                    {
                        putByte(CONTEXT_PTR, CONTEXT_PTR->Chr, &i, &w, &p, &c1);
                    }
                    break;
                case SDL_KEYDOWN:
				    if (event.key.keysym.sym == SDLK_RETURN)
                    {
                        printf("You enered ENTERED: ");
                        any y = popSym(CONTEXT_PTR, i, w, p, &c1);
                        print(CONTEXT_PTR, y);
                        printf("\n");
                        i = 0;
                        load(CONTEXT_PTR, NULL, 0, y);


                        // Push(c1, parse(CONTEXT_PTR, y, YES));
                        // x = evList(CONTEXT_PTR, data(c1));
                        // drop(c1);

                        // putByte0(&i, &w, &p);
                    }
                    //SDL_StartTextInput();
                    break;
            }
        }

        // generate random a screen position
        int x = rand() % SCREEN_SIZE;
        int y = rand() % SCREEN_SIZE;
        // generate a random screen color
        SDL_Color color;
        color.r = rand() % 255;
        color.g = rand() % 255;
        color.b = rand() % 255;
        surface = SDL_GetWindowSurface(window);
        drawPixel (surface, x, y, color);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

