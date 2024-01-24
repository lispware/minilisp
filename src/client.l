(load "SDL.l")

(setq WIN (SDL_CreateWindow "HELLO SDL" 800 600))
(setq RENDERER (SDL_CreateRenderer WIN))

(SDL_Init)
(setq LOOP (uv_loop))

(de getColor (BUF)
	(let (A (chop BUF)
	      B (split A ",")
	      C (mapcar pack B)
	      D (mapcar format C)
	      )
	D))

(de ON_READ (BUF TCP_AND_MP)
    (let (TCP (car TCP_AND_MP)
          MP (car (cdr TCP_AND_MP)))
    (uv_close TCP)
	(SDL_PushEvent (list MP BUF))))

(de ON_WRITE (TCP MOUSE_POSITION)
    (let TCP_AND_MP (list TCP MOUSE_POSITION)
		(uv_read_start TCP BUF TCP_AND_MP (ON_READ BUF TCP_AND_MP))))

(de process (TCP DATA)
	(uv_tcp_write TCP  (pack (car DATA) "," (cdr DATA)) DATA (ON_WRITE TCP DATA)))

(de TASK (DATA)
	(uv_tcp_connect LOOP "127.0.0.1" 8080 TCP DATA (process TCP DATA)))

(loop
    (while (SDL_PollEvent)
        (let (EVENT  @)
        	(if (COMP_PACK (car EVENT) (PACK SDL_MOUSEBUTTONDOWN))
        	    (do 1
        	    	(let (MP (SDL_GetMouseState))
        	    		(uv_queue_work LOOP (TASK MP))
						(SDL_SetRenderDrawColor RENDERER 255 255 255)
						(SDL_RenderDrawLine RENDERER 10 10 (car MP) (cdr MP))
						(SDL_RenderPresent RENDERER))))

        	(if (COMP_PACK (car EVENT) (PACK SDL_USEREVENT))
        	    (do 1
					(let (
					      ED (car (cdr EVENT))
					      BUF (car (cdr ED))
					      RGB (getColor BUF)
					      R (car RGB)
					      G (car (cdr RGB))
					      B (car (cdr (cdr RGB)))
						  X1 (car (car ED))
					      Y1 (cdr (car ED)))
						(SDL_SetRenderDrawColor RENDERER R G B)
						(SDL_RenderDrawLine RENDERER 10 10 X1 Y1))
					(SDL_RenderPresent RENDERER)
				))

        	(if (and (COMP_PACK (car EVENT) (PACK SDL_WINDOWEVENT)) (COMP_PACK (car (cdr EVENT)) (PACK SDL_WINDOWEVENT_CLOSE)))
        		(do 1 (SDL_DestroyWindow WIN) (uv_stop LOOP)(uv_run_nowait LOOP) (SDL_Quit)  (bye)))
            )
        )

    	(uv_run_nowait LOOP)
    )


(de dummy ()
	(let X 10
	(setq X (calloc 200))))

(let X (calloc 100)
	(finally (free X)
		(dummy)
		(prinl "DO SOMETHING")))
	
