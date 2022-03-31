#include <lisp.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#define SCREEN_SIZE 1000

TTF_Font* font;
SDL_Texture *texture, *text;
SDL_Renderer* renderer;



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

SDL_Event LISP_SDL_EVENT;

SDL_Window* LISP_SDL_WINDOW = NULL;

any sdlGetEvent(Context *CONTEXT_PTR, any x)
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

any sdlIsCloseEvent(Context *CONTEXT_PTR, any x)
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

any sdlIsTextInput(Context *CONTEXT_PTR, any x)
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

any sdlCloseWindow(Context *CONTEXT_PTR, any x)
{
    SDL_DestroyWindow(LISP_SDL_WINDOW);
    SDL_Quit();
    return Nil;
}

any sdlIsEnterPressed(Context *CONTEXT_PTR, any x)
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

int main(int argc, char* av[])
{

    int i=0;
    uword w;
    any x, y;
    cell c1, *p;
    int bufPTR=0;
    char buf[1024]={0};


    Context *CONTEXT_PTR = &LISP_CONTEXT;
    y = Nil;
    setupBuiltinFunctions(&CONTEXT_PTR->Mem);
    addBuiltinFunction(&CONTEXT_PTR->Mem, "sdlpoll", sdlGetEvent);
    addBuiltinFunction(&CONTEXT_PTR->Mem, "sdlisclose", sdlIsCloseEvent);
    addBuiltinFunction(&CONTEXT_PTR->Mem, "sdlclose", sdlCloseWindow);
    addBuiltinFunction(&CONTEXT_PTR->Mem, "sdlistextinput", sdlIsTextInput);
    addBuiltinFunction(&CONTEXT_PTR->Mem, "sdisenterpressed", sdlIsEnterPressed);


    initialize_context(CONTEXT_PTR);
    av++;
    CONTEXT_PTR->AV = av;

    CONTEXT_PTR->InFile = stdin, CONTEXT_PTR->Env.get = getStdin;
    CONTEXT_PTR->OutFile = stdout, CONTEXT_PTR->Env.put = putStdout;
    CONTEXT_PTR->ApplyArgs = Nil;
    CONTEXT_PTR->ApplyBody = Nil;

    SDL_Window* LISP_SDL_WINDOW = NULL;
    SDL_Surface* surface = NULL;


    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return 1;
    }

	if ( TTF_Init() < 0 )
    {
        printf("Could not initialize font\n");
	}


    LISP_SDL_WINDOW = SDL_CreateWindow
        (
         "Graphics Demo",
         SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
         SCREEN_SIZE, SCREEN_SIZE,
         SDL_WINDOW_SHOWN
        );

    if (LISP_SDL_WINDOW == NULL)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return 1;
    }

	renderer = SDL_CreateRenderer( LISP_SDL_WINDOW, -1, SDL_RENDERER_ACCELERATED );

	font = TTF_OpenFont("font.ttf", 20);

	if ( !font ) {
		printf("Error loading font1: %d\n", TTF_GetError());
	}

    //surface = SDL_GetWindowSurface(LISP_SDL_WINDOW);
    //SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0xFF, 0x00, 0x00, 0x00));

    Uint32 delay = (33 / 10) * 10;  /* To round it down to the nearest 10 ms */
    SDL_TimerID my_timer_id = SDL_AddTimer(delay, timerFunc, NULL);

    SDL_StartTextInput();

    loadAll(CONTEXT_PTR, NULL);


#if 0
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
                            SDL_DestroyWindow(LISP_SDL_WINDOW);
                            SDL_Quit();
                            return 0;
                        default:
                            break;
                    }
                    break;
                case SDL_USEREVENT:
                    SDL_UpdateWindowSurface(LISP_SDL_WINDOW);
                    //RENDERER = loadRenderer();
                    break;

                case SDL_TEXTINPUT:
                    printf("AA\n");
                    CONTEXT_PTR->Chr = ((char *)event.text.text)[0];
                    if (i == 0)
                    {
                        putByte1(CONTEXT_PTR->Chr, &i, &w, &p);
                        buf[bufPTR++]=CONTEXT_PTR->Chr;
                    }
                    else
                    {
                        putByte(CONTEXT_PTR, CONTEXT_PTR->Chr, &i, &w, &p, &c1);
                        buf[bufPTR++]=CONTEXT_PTR->Chr;
                    }
                    break;
                case SDL_KEYDOWN:
                    if (i && event.key.keysym.sym == SDLK_RETURN)
                    {
                        printf("You enered ENTERED: ");
                        y = popSym(CONTEXT_PTR, i, w, p, &c1);
                        print(CONTEXT_PTR, y);
                        printf("\n");
                        i = 0;
                        load(CONTEXT_PTR, NULL, 0, y);
                        bufPTR=0;
                        for(int i = 0 ; i < 1024; i++) buf[i]=0;
                        SDL_RenderClear( renderer );

                    }
                    break;
                case SDL_KEYUP:
                    break;
            }
        }

        // RENDER TEXT
        //
        if (bufPTR)
        {
            renderText(buf);
        }
        else
        {
            renderText("____________________________________");
        }
        ////////////////////////////////////////////////////////////////////////////////




        // // generate random a screen position
        // int x = rand() % SCREEN_SIZE;
        // int y = rand() % SCREEN_SIZE;
        // // generate a random screen color
        // SDL_Color color;
        // color.r = rand() % 255;
        // color.g = rand() % 255;
        // color.b = rand() % 255;
        // surface = SDL_GetWindowSurface(LISP_SDL_WINDOW);
        // drawPixel (surface, x, y, color);
    }
#endif

    SDL_DestroyWindow(LISP_SDL_WINDOW);
    SDL_Quit();
    return 0;
}


void renderText(char *buf)
{
    SDL_Rect dest;
    SDL_Color foreground = { 0xff, 0xff, 0xff };
    SDL_Surface* text_surf = TTF_RenderText_Solid(font, buf, foreground);
    text = SDL_CreateTextureFromSurface(renderer, text_surf);

    dest.x = 10;
    dest.y = 100;
    dest.w = text_surf->w;
    dest.h = text_surf->h;
    SDL_RenderCopy(renderer, text, NULL, &dest);

    SDL_DestroyTexture(text);
    SDL_FreeSurface(text_surf);

    SDL_RenderPresent( renderer );
}

