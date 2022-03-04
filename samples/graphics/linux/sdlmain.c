#include <SDL2/SDL.h>
#include <stdio.h>

#define SCREEN_SIZE 500

typedef struct
{
    int width;
    int height;
    unsigned char *pixels;
} INTEROP;

typedef int (*RENDERER_TYPE)(INTEROP*);

RENDERER_TYPE RENDERER;


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

int main(int argc, char* args[])
{
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

    surface = SDL_GetWindowSurface(window);
    SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0xFF, 0x00, 0x00, 0x00));
    INTEROP interop;
    interop.pixels = surface->pixels;
    interop.width = SCREEN_SIZE;
    interop.height = SCREEN_SIZE;
    
    //RENDERER = loadRenderer();

    Uint32 delay = (33 / 10) * 10;  /* To round it down to the nearest 10 ms */
    SDL_TimerID my_timer_id = SDL_AddTimer(delay, timerFunc, NULL);
    
    SDL_Event event;
    //while(RENDERER(&interop))
    while(1)
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
            }
        }
        printf("HERE\n");
        getchar();
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

