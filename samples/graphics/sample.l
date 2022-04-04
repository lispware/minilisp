(sdlCreateWindow "HELLO" 500 500 255 0 0)

(de reverse (L)
    (let R Nil
        (for I L
            (setq R (cons I R)))
    R))


(de box (X Y S)
    (for I S
        (sdlputpixel (- (+ X I) 1)  Y 255 255 255)
        (sdlputpixel (- (+ X I) 1) (+ Y S) 255 255 255)

        (sdlputpixel X (- (+ Y I) 1) 255 255 255)
        (sdlputpixel (+ X S) (- (+ Y I) 1) 255 255 255))
    (sdlupdatewindow)
    )

(loop
    (while (sdlpoll)
        (let X (sdlistextinput)
            (if X (prinl (reverse (setq R (cons (char X) R)))) )


        (if (sdisbackspacepressed) (do 1 (setq R (cdr R)) (prinl (reverse R))))

        (if (sdisenterpressed) (do 1 (if R (prinl "ENTER PRESSED: "  (load (pack (reverse R))))) (setq R Nil) ))
        (if (sdlisclose) (sdlclose) ))))



# #if 0
# 
#     //SDL_FillRect(LISP_SDL_SURFACE, NULL, SDL_MapRGBA(LISP_SDL_SURFACE->format, 0x00, 0x00, 0x00, 0x00));
#     //                SDL_UpdateWindowSurface(LISP_SDL_WINDOW);
#     SDL_Event event;
#     for(;;)
#     {
#         while( SDL_PollEvent( &event ) )
#         {
#             switch( event.type )
#             {
#                 case SDL_WINDOWEVENT:
#                     switch (event.window.event)
#                     {
#                         case SDL_WINDOWEVENT_CLOSE:
#                             SDL_DestroyWindow(LISP_SDL_WINDOW);
#                             SDL_Quit();
#                             return 0;
#                         default:
#                             break;
#                     }
#                     break;
#                 case SDL_USEREVENT:
#                     SDL_UpdateWindowSurface(LISP_SDL_WINDOW);
#                     //RENDERER = loadRenderer();
#                     break;
# 
#                 case SDL_TEXTINPUT:
#                     printf("AA\n");
#                     CONTEXT_PTR->Chr = ((char *)event.text.text)[0];
#                     if (i == 0)
#                     {
#                         putByte1(CONTEXT_PTR->Chr, &i, &w, &p);
#                         buf[bufPTR++]=CONTEXT_PTR->Chr;
#                     }
#                     else
#                     {
#                         putByte(CONTEXT_PTR, CONTEXT_PTR->Chr, &i, &w, &p, &c1);
#                         buf[bufPTR++]=CONTEXT_PTR->Chr;
#                     }
#                     break;
#                 case SDL_KEYDOWN:
#                     if (i && event.key.keysym.sym == SDLK_RETURN)
#                     {
#                         printf("You enered ENTERED: ");
#                         y = popSym(CONTEXT_PTR, i, w, p, &c1);
#                         print(CONTEXT_PTR, y);
#                         printf("\n");
#                         i = 0;
#                         load(CONTEXT_PTR, NULL, 0, y);
#                         bufPTR=0;
#                         for(int i = 0 ; i < 1024; i++) buf[i]=0;
#                         SDL_RenderClear( renderer );
# 
#                     }
#                     break;
#                 case SDL_KEYUP:
#                     break;
#             }
#         }
# 
#         // RENDER TEXT
#         //
#         if (bufPTR)
#         {
#             renderText(buf);
#         }
#         else
#         {
#             renderText("____________________________________");
#         }
#         ////////////////////////////////////////////////////////////////////////////////
# 
# 
# 
# 
#         // generate random a screen position
#         int x = rand() % SCREEN_SIZE;
#         int y = rand() % SCREEN_SIZE;
#         // generate a random screen color
#         SDL_Color color;
#         color.r = rand() % 255;
#         color.g = rand() % 255;
#         color.b = rand() % 255;
#         LISP_SDL_SURFACE = SDL_GetWindowSurface(LISP_SDL_WINDOW);
#         drawPixel (LISP_SDL_SURFACE, x, y, color);
#                     SDL_UpdateWindowSurface(LISP_SDL_WINDOW);
#     }
# #endif
