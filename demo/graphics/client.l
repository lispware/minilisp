(load "SDL.l")

(setq WIN (SDL_CreateWindow "MiniPicoLisp" 1000 1000))
#(setq WIN_SURFACE (SDL_GetWindowSurface WIN))
(setq RENDERER (SDL_CreateRenderer WIN))

(SDL_SetRenderDrawBlendMode RENDERER 1)

(SDL_Init)

(setq LOOP (uv_loop))
# convert "1,2,3" -> (1,2,3)
(de getColor (BUF)
   (let
      (A (chop BUF)
         B (split A ",")
         C (mapcar pack B)
         D (mapcar format C) )
      D ) )

(de drawBox (X Y)
   (let
      (W 20
         X1 (- X W)
         Y1 (- Y W)
         X2 (+ X W)
         Y2 (+ Y W) )
      (SDL_RenderDrawLine RENDERER X1 Y1 X2 Y1)
      (SDL_RenderDrawLine RENDERER X1 Y1 X1 Y2)
      (SDL_RenderDrawLine RENDERER X1 Y2 X2 Y2)
      (SDL_RenderDrawLine RENDERER X2 Y1 X2 Y2) ) )

(de ON_READ (BUF TCP_AND_MP)
   (let (TCP (car TCP_AND_MP)  MP (car (cdr TCP_AND_MP)))
      (uv_close TCP)
      (SDL_PushEvent (list MP BUF)) ) )

(de ON_WRITE (TCP MOUSE_POSITION)
   (let TCP_AND_MP (list TCP MOUSE_POSITION)
      (uv_read_start TCP BUF TCP_AND_MP (ON_READ BUF TCP_AND_MP)) ) )

(de process (TCP DATA)
   (uv_tcp_write
      TCP
      (pack (car DATA) "," (cdr DATA))
      DATA
      (ON_WRITE TCP DATA) ) )

(de TASK (DATA)
   (uv_tcp_connect LOOP "127.0.0.1" 8080 TCP DATA (process TCP DATA)) )

(setq FS_EVENT_HANDLER (uv_fs_event_start LOOP "refresh.l" FN (file-change FN)))

(de file-change (FN)
   (uv_fs_event_stop FS_EVENT_HANDLER)
   (prinl "FILE CHANGED")
   (load "refresh.l")
   (setq FS_EVENT_HANDLER (uv_fs_event_start LOOP "refresh.l" FN (file-change FN))) )

(setq FONT_OFFSET_X 28)

(setq FONT_OFFSET_Y 17)

(setq FONT_WIDTH 21)

(setq FONT_HEIGHT 43)

(setq FONT
   (IMG_Load (pack "font_" FONT_WIDTH "x" FONT_HEIGHT "_" FONT_OFFSET_X "x" FONT_OFFSET_Y ".bmp")) )

(SDL_SetTransparency WIN FONT 0 0 0)

(setq FONT_TEXTURE (SDL_CreateTextureFromSurface RENDERER FONT))

(SDL_FreeSurface FONT)

(SDL_SetWindowPosition WIN 100 1000)

(load "refresh.l")

(loop
   (while (SDL_PollEvent)
      (let (EVENT @)
         (if (COMP_PACK (car EVENT) (PACK SDL_MOUSEBUTTONDOWN))
            (do 1
               (let (MP (SDL_GetMouseState))
                  (uv_queue_work LOOP (TASK MP))
                  (SDL_SetRenderDrawColor RENDERER 0 255 0 127)
                  (drawBox (car MP) (cdr MP))
                  (SDL_RenderFillRect
                     RENDERER
                     (- (car MP) 20)
                     (- (cdr MP) 20)
                     40
                     40 )
                  (SDL_SetRenderDrawColor RENDERER 255 255 0 255)
                  (SDL_RenderFillRect
                     RENDERER
                     (+ (car MP) 0)
                     (+ (cdr MP) 0)
                     40
                     40 )
                  (SDL_RenderDrawPoint RENDERER (car MP) (cdr MP))
                  (SDL_RenderPresent RENDERER) ) ) )
         (if (COMP_PACK (car EVENT) (PACK SDL_USEREVENT))
            (do 1
               (let
                  (ED (car (cdr EVENT))
                     BUF (car (cdr ED))
                     RGB (getColor BUF)
                     R (car RGB)
                     G (car (cdr RGB))
                     B (car (cdr (cdr RGB)))
                     X1 (car (car ED))
                     Y1 (cdr (car ED)) )
                  (SDL_SetRenderDrawColor RENDERER R G B)
                  (drawBox X1 Y1) )
               (SDL_RenderPresent RENDERER) ) )
         (if
            (and
               (COMP_PACK (car EVENT) (PACK SDL_WINDOWEVENT))
               (COMP_PACK (car (cdr EVENT)) (PACK SDL_WINDOWEVENT_CLOSE)) )
            (do 1
               (SDL_DestroyWindow WIN)
               (uv_stop LOOP)
               (uv_run_nowait LOOP)
               (SDL_Quit)
               (bye) ) ) ) )
   (uv_run_nowait LOOP) )

(de dummy NIL
   (let X 10
      (setq X (calloc 200)) ) )

(let X (calloc 100)
   (finally (free X)
      (dummy)
      (prinl "DO SOMETHING") ) )

