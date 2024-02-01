(load "SDL.l")

(setq WIN (SDL_CreateWindow "MiniPicoLisp" 1000 1000))
(setq RENDERER (SDL_CreateRenderer WIN))
(SDL_SetRenderDrawBlendMode RENDERER 1)
(SDL_Init)

(setq LOOP (uv_loop))

(de create-handler (fileName N)
	(let (CODE1 (pack
					"-setq FS_EVENT_HANDLER" N " (uv_fs_event_start LOOP \"" fileName "\" FN (file-change" I " FN))")
	      CODE2 (pack
				"-de file-change" N  "(FileName)"
   					"(uv_fs_event_stop FS_EVENT_HANDLER" N ")"
   					#"(prinl FileName \" is changed\")"
   					"(load FileName)"
   					"(setq FS_EVENT_HANDLER" N " (uv_fs_event_start LOOP FileName FN (file-change" I " FN)))"))
   		(load CODE1)
   		(load CODE2)))


(for I 9
	(create-handler (pack "page" I ".l") I))

(setq FONT_OFFSET_X 28)
(setq FONT_OFFSET_Y 17)
(setq FONT_WIDTH 21)
(setq FONT_HEIGHT 43)
(setq FONT
   (IMG_Load (pack "font_" FONT_WIDTH "x" FONT_HEIGHT "_" FONT_OFFSET_X "x" FONT_OFFSET_Y ".bmp")) )
(SDL_SetTransparency WIN FONT 0 0 0)
(setq FONT_TEXTURE (SDL_CreateTextureFromSurface RENDERER FONT))
(SDL_FreeSurface FONT)

(SDL_SetWindowPosition WIN 0 0)

(load "lib.l")
(load "page1.l")

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
   			   #(uv_fs_event_stop FS_EVENT_HANDLER)
               (SDL_DestroyWindow WIN)
               (uv_run_nowait LOOP)
               (uv_stop LOOP)
               (SDL_Quit)
               (bye) ) ) ) )
   (uv_run_nowait LOOP) )

