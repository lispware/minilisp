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

(de ON_WRITE (TTT DATA)
	(prinl "ON_WRITE CALLED TCP => " TTT)
	(prinl "ON_WRITE CALLED DATA => " DATA)

	(uv_tcp_read TTT DDD DATA (ON_READ DDD DATA))


	)

(de process (TTT DATA)
	(prinl "HURRAY " TTT " DATA = " DATA)

	(uv_tcp_write TTT  "APPLE MANGO BANANA" DATA (ON_WRITE TTT DATA))


	)

(de TASK (DATA)

	(prinl "HERE 12345 " DATA)
	(uv_tcp_connect LOOP "127.0.0.1" 8080 XXX DATA (process XXX DATA))

	)

(loop

    (while (SDL_PollEvent)
        (let ( EVENT  @)

            (prinl "TYPE = " (car EVENT) ", EVENT = " (cdr EVENT) " CLOSE = " SDL_WINDOWEVENT_CLOSE)
            (prinl "TYPE = " (car EVENT) ", EVENT = " (cdr EVENT) " MOUSE = " SDL_MOUSEBUTTONDOWN)

        	(if (COMP_PACK (car EVENT) (PACK SDL_MOUSEBUTTONDOWN))
        	    (do 1
        	    (let DATA (SDL_GetMouseState) (uv_queue_work LOOP (TASK DATA)))
				(SDL_SetRenderDrawColor R 255 255 0)
				(let P (SDL_GetMouseState)
				(SDL_RenderDrawLine R 10 10 (car P) (cdr P)))
				(SDL_RenderPresent R)))

        	(if (COMP_PACK (car EVENT) (PACK SDL_USEREVENT))
        	    (do 1
        	    	(prinl "GOT THE USER EVENT POINT X" (car (car (car (cdr EVENT)))))
        	    	(prinl "GOT THE USER EVENT POINT Y" (cdr (car (car (cdr EVENT)))))
        	    	(prinl "GOT THE USER EVENT STR " (cdr (car (cdr EVENT))))
					(SDL_SetRenderDrawColor R 0 255 0)
					(let (X1 (car (car (car (cdr EVENT)))) Y1 (cdr (car (car (cdr EVENT))))) (SDL_RenderDrawLine R 10 10 X1 Y1))
					(SDL_RenderPresent R)
				))

        	(if (COMP_PACK (car (cdr EVENT)) (PACK SDL_WINDOWEVENT_CLOSE))
        		(prinl "IT IS CLOSE")
        		(prinl "IT IS NOT CLOSE"))

        	(if (and (COMP_PACK (car EVENT) (PACK SDL_WINDOWEVENT)) (COMP_PACK (car (cdr EVENT)) (PACK SDL_WINDOWEVENT_CLOSE)))
        		(do 1 (SDL_DestroyWindow W) (SDL_Quit) (bye)))


            )
        )

    	(uv_run_nowait LOOP)
    )

