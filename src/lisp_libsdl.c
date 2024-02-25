#ifndef DONT_USE_LIBSDL

#include <SDL.h>
#include "pico.h"

any LISP_SDL_PushEvent(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));

    SDL_Event event;
    event.type = SDL_USEREVENT;
    event.user.data1 = p1;
    SDL_PushEvent(&event);
}

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

// (LISP_SDL_SetTransparency WIN TEXTURE R G B) 
any LISP_SDL_SetTransparency(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));
    UNPACK(p1, _w);
    SDL_Window *window = (SDL_Window*)_w;

    x = cdr(x);
    any p2 = EVAL(car(x));
    UNPACK(p2, _t);
    SDL_Texture *texture = (SDL_Texture*)_t;

    x = cdr(x);
    any p3 = EVAL(car(x));
    int R = unBox(p3);

    x = cdr(x);
    any p4 = EVAL(car(x));
    int G = unBox(p4);

    x = cdr(x);
    any p5 = EVAL(car(x));
    int B = unBox(p5);

    SDL_PixelFormat* textureFormat = SDL_AllocFormat(SDL_GetWindowPixelFormat(window));
    Uint32 transparentColorKey = SDL_MapRGB(textureFormat, R, G, B);
    SDL_SetColorKey(texture, SDL_TRUE, transparentColorKey);

    return Nil;
}

any LISP_SDL_RenderClear(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));

    UNPACK(p1, w);
    SDL_Renderer *renderer = (SDL_Renderer*)w;
    SDL_RenderClear(renderer);
    return Nil;
}

any LISP_SDL_GetWindowSurface(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));

    UNPACK(p1, w);
    SDL_Window *window = (SDL_Window*)w;
    SDL_Renderer *surface = SDL_GetWindowSurface(window);
    PACK(surface, P);
    return P;
}

typedef struct _abcd {
    uint16_t bfType;        // Signature, must be "BM" (0x42, 0x4D)
    uint32_t bfSize;        // File size in bytes
    uint16_t bfReserved1;   // Reserved, must be 0
    uint16_t bfReserved2;   // Reserved, must be 0
    uint32_t bfOffBits;     // Offset to the pixel data
    uint32_t biSize;         // Size of the header
    int32_t biWidth;         // Image width in pixels
    int32_t biHeight;        // Image height in pixels
    uint16_t biPlanes;       // Number of color planes (must be 1)
    uint16_t biBitCount;     // Number of bits per pixel (24 bits for RGB)
    uint32_t biCompression;  // Compression method (0 for uncompressed)
    uint32_t biSizeImage;    // Image size in bytes (can be 0 if uncompressed)
    int32_t biXPelsPerMeter; // Horizontal resolution (pixels per meter)
    int32_t biYPelsPerMeter; // Vertical resolution (pixels per meter)
    uint32_t biClrUsed;      // Number of colors in the palette (0 for 24-bit)
    uint32_t biClrImportant; // Number of important colors (0 for all)
} BMPHeader;

unsigned char* extractChannelFromBMP(const char* filename, int* width, int* height, int* bpp, int* PADDING)
{
    FILE* file = fopen(filename, "rb");
    if (!file)
    {
        perror("Error opening file");
        return NULL;
    }

    // Read the BMP header (54 bytes)
    unsigned char header[54];
    fread(header, 1, 54, file);

    // Check if it's a valid BMP file
    if (header[0] != 'B' || header[1] != 'M')
    {
        fclose(file);
        printf("Not a BMP file\n");
        return NULL;
    }

    // Extract width and height from the header
    *width = *(int*)&header[18];
    *height = *(int*)&header[22];
    int fileSize = *(int*)&header[2];


    // Calculate the "Bytes per Pixel" (BPP)
    *bpp = *(unsigned short*)&header[28] / 8;


    // Calculate the size of the image data
    int imageSize = *width * *height * *bpp;

    *PADDING = (fileSize - imageSize - 54) / *height;

    // Allocate memory for the image data
    unsigned char* imageData = (unsigned char*)malloc(fileSize - 54);
    if (!imageData)
    {
        perror("Error allocating memory for image data");
        fclose(file);
        return NULL;
    }

    // Read the image data
    fread(imageData, sizeof(unsigned char), fileSize-54, file);
    fclose(file);

    return imageData;
}

any LISP_IMG_Load(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));
    char *fileName = (char *)calloc(bufSize(p1), 1);
    bufString(p1, fileName);

    int width, height, bpp, PADDING;

    unsigned char *imagePixels = extractChannelFromBMP(fileName, &width, &height, &bpp, &PADDING);
    printf("Image size = %d %d\n", width, height);

    SDL_Surface *imageSurface =  SDL_CreateRGBSurface(0, width, height, 32, 0xFF0000, 0xFF00, 0xFF, 0);

    unsigned char *pixels = (unsigned char *)imageSurface->pixels;
    int pi = 0;
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            int o = ((height - i - 1) * width * 4) + (j * 4);
            pixels[o++] = imagePixels[pi++];
            pixels[o++] = imagePixels[pi++];
            pixels[o++] = imagePixels[pi++];
            pixels[o++] = 0;
        }
        pi+=PADDING;
    }

    PACK(imageSurface, P);
    return P;
}

any LISP_SDL_CreateTextureFromSurface(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));
    UNPACK(p1, _r);
    SDL_Renderer *renderer = (SDL_Renderer*)_r;

    x = cdr(x);
    any p2 = EVAL(car(x));
    UNPACK(p2, _s);
    SDL_Surface *surface = (SDL_Renderer*)_s;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    PACK(texture, P);
    return P;
}

any LISP_SDL_FreeSurface(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));
    UNPACK(p1, _s);
    SDL_Surface *surface = (SDL_Surface*)_s;

    SDL_FreeSurface(surface);
    return Nil;
}

any LISP_SDL_RenderCopy(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));
    UNPACK(p1, _r);
    SDL_Renderer *renderer = (SDL_Renderer*)_r;

    x = cdr(x);
    any p2 = EVAL(car(x));
    UNPACK(p2, _t);
    SDL_Texture *texture = (SDL_Texture*)_t;

    if (cdr(x) == Nil)
    {
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        return Nil;
    }

    x = cdr(x);
    any sx = unBox(EVAL(car(x)));
    x = cdr(x);
    any sy = unBox(EVAL(car(x)));
    x = cdr(x);
    any sw = unBox(EVAL(car(x)));
    x = cdr(x);
    any sh = unBox(EVAL(car(x)));
    x = cdr(x);
    any dx = unBox(EVAL(car(x)));
    x = cdr(x);
    any dy = unBox(EVAL(car(x)));
    x = cdr(x);
    any dw = unBox(EVAL(car(x)));
    x = cdr(x);
    any dh = unBox(EVAL(car(x)));

    SDL_Rect src;
    src.x = sx;
    src.y = sy;
    src.w = sw;
    src.h = sh;
    SDL_Rect dst;
    dst.x = dx;
    dst.y = dy;
    dst.w = dw;
    dst.h = dh;

    SDL_RenderCopy(renderer, texture, &src, &dst);

    return Nil;
}

any LISP_SDL_SetWindowSize(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));

    x = cdr(x);
    any p2 = EVAL(car(x));
    x = cdr(x);
    any p3 = EVAL(car(x));

    UNPACK(p1, w);
    SDL_Window *window = (SDL_Window*)w;

    word W = unBox(p2);
    word H = unBox(p3);

    SDL_SetWindowSize(window, W, H);


    return Nil;
}

any LISP_SDL_SetWindowPosition(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));

    x = cdr(x);
    any p2 = EVAL(car(x));
    x = cdr(x);
    any p3 = EVAL(car(x));

    UNPACK(p1, w);
    SDL_Window *window = (SDL_Window*)w;

    word X = unBox(p2);
    word Y = unBox(p3);

    SDL_SetWindowPosition(window, X, Y);

    return Nil;
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
            PACK(event.window.event, eventValue);
            y = cdr(y) = cons(eventValue, Nil);
            return Pop(c1);
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            any y;
            cell c1;

            PACK(event.type, eventType);
            Push(c1, y = cons(eventType, Nil));
            PACK(event.window.event, eventValue);
            y = cdr(y) = cons(eventValue, Nil);
            return Pop(c1);
        }
        else if (event.type == SDL_USEREVENT)
        {
            any y;
            cell c1;

            PACK(event.type, eventType);
            Push(c1, y = cons(eventType, Nil));
            y = cdr(y) = cons(event.user.data1, Nil);
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

any LISP_SDL_RenderDrawPoint(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));
    UNPACK(p1, renderer);
    x = cdr(x);
    any X1 = EVAL(car(x));
    x = cdr(x);
    any Y1 = EVAL(car(x));

    SDL_RenderDrawPoint((SDL_Renderer*)renderer, unBox(X1), unBox(Y1));

    return Nil;
}

// (SDL_SetRenderDrawBlendMode RENDERER MODE)
// 0 SDL_BLENDMODE_NONE
// 1 SDL_BLENDMODE_BLEND
// 2 SDL_BLENDMODE_ADD
// 3 SDL_BLENDMODE_MOD
// 4 SDL_BLENDMODE_MUL
any LISP_SDL_SetRenderDrawBlendMode(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));
    UNPACK(p1, renderer);
    x = cdr(x);
    any M = EVAL(car(x));
    int _mode = M == Nil? 0 : unBox(M);
    SDL_BlendMode modes[] = {SDL_BLENDMODE_NONE,SDL_BLENDMODE_BLEND,SDL_BLENDMODE_ADD,SDL_BLENDMODE_MOD,SDL_BLENDMODE_MUL};

    SDL_SetRenderDrawBlendMode((SDL_Renderer*)renderer, modes[_mode]);

    return Nil;
}

any LISP_SDL_SetTextureBlendMode(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));
    UNPACK(p1, texture);
    x = cdr(x);
    any M = EVAL(car(x));
    int _mode = M == Nil? 0 : unBox(M);
    SDL_BlendMode modes[] = {SDL_BLENDMODE_NONE,SDL_BLENDMODE_BLEND,SDL_BLENDMODE_ADD,SDL_BLENDMODE_MOD,SDL_BLENDMODE_MUL};

    SDL_SetTextureBlendMode(texture, modes[_mode]);

    return Nil;
}

any LISP_SDL_SetTextureAlphaMod(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));
    UNPACK(p1, texture);
    x = cdr(x);
    any M = EVAL(car(x));
    int alpha = M == Nil? 0 : unBox(M);

    SDL_SetTextureAlphaMod(texture, alpha);

    return Nil;
}

any LISP_SDL_RenderFillRect(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));
    UNPACK(p1, renderer);
    x = cdr(x);
    any X = EVAL(car(x));
    x = cdr(x);
    any Y = EVAL(car(x));
    x = cdr(x);
    any W = EVAL(car(x));
    x = cdr(x);
    any H = EVAL(car(x));

    SDL_Rect rect;
    rect.x = unBox(X);
    rect.y = unBox(Y);
    rect.w = unBox(W);
    rect.h = unBox(H);
    SDL_RenderFillRect((SDL_Renderer*)renderer, &rect);

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

    SDL_RenderDrawLine((SDL_Renderer*)renderer, unBox(X1), unBox(Y1), unBox(X2), unBox(Y2));

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
    x = cdr(x);
    any A = EVAL(car(x));

    SDL_SetRenderDrawColor((SDL_Renderer*)renderer, unBox(R), unBox(G), unBox(B), A == Nil ? SDL_ALPHA_OPAQUE: unBox(A));

    return Nil;
}

any LISP_SDL_GetMouseState(any ex)
{
    Uint32 mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    return cons(box(mouseX), box(mouseY));
}

#endif //DONT_USE_LIBSDL
