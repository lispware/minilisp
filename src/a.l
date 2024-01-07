(load "SDL.l")

(de dummy ()
	(let X 10
	(setq X (calloc 200))))

(let X (calloc 100)
	(finally (free X)
		(dummy)
		(prinl "DO SOMETHING")))
	

(setq W (SDL_CreateWindow "HELLO SDL" 800 600))
(setq R (SDL_CreateRenderer W))


(SDL_Init)
(setq LOOP (uv_loop))

(de ON_READ (D D2)
	(prinl "ON_READ123 ---> <<<" D2 ">>>")
	(prinl)
	(prinl D)
	(prinl "---------------------------------------")

	(SDL_PushEvent (list D2 D ))

	)

(de ON_WRITE (TCP DATA)
	(uv_read_start TCP BUF DATA (ON_READ BUF DATA)))

(de process (TTT DATA)
	(uv_tcp_write TTT  "APPLE MANGO BANANA" DATA (ON_WRITE TTT DATA)))

(de TASK (DATA)
	(uv_tcp_connect LOOP "127.0.0.1" 8080 TCP DATA (process TCP DATA)))

(loop

    (while (SDL_PollEvent)
        (let ( EVENT  @)

        	(if (COMP_PACK (car EVENT) (PACK SDL_MOUSEBUTTONDOWN))
        	    (do 1
        	    (let (DATA (SDL_GetMouseState) DATA2 DATA )(uv_queue_work LOOP (TASK DATA DATA2)))
				(SDL_SetRenderDrawColor R 255 255 0)
				(let P (SDL_GetMouseState)
				(SDL_RenderDrawLine R 10 10 (car P) (cdr P)))
				(SDL_RenderPresent R)))

        	(if (COMP_PACK (car EVENT) (PACK SDL_USEREVENT))
        	    (do 1
					(SDL_SetRenderDrawColor R 0 255 0)
					(let (X1 (car (car (car (cdr EVENT))))
					      Y1 (cdr (car (car (cdr EVENT)))))
						(SDL_RenderDrawLine R 10 10 X1 Y1))
					(SDL_RenderPresent R)
				))

        	(if (COMP_PACK (car (cdr EVENT)) (PACK SDL_WINDOWEVENT_CLOSE))
        		(prinl "IT IS CLOSE")
        		(prinl "IT IS NOT CLOSE"))

        	(if (and (COMP_PACK (car EVENT) (PACK SDL_WINDOWEVENT)) (COMP_PACK (car (cdr EVENT)) (PACK SDL_WINDOWEVENT_CLOSE)))
        		(do 1 (SDL_DestroyWindow W) (uv_stop LOOP)(uv_run_nowait LOOP) (SDL_Quit)  (bye)))


            )
        )

    	(uv_run_nowait LOOP)
    )

