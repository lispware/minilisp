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

(de ON_READ (S)
	(prinl "WHY ON_READ CALLED ---> " S)
	(prinl)
	(prinl "---------------------------------------")

	(SDL_PushEvent S)

	)

(de ON_WRITE ()
	(prinl "ON_WRITE CALLED")


	)

(de ON_CONNECT (TCP)
	(prinl "ON _CONNECT CALLED ") 

	(uv_tcp_write TCP "She sells sea shell  on the sea shore")

	(uv_tcp_read TCP )
)

(de TASK ()

	(prinl "HERE")
	(uv_tcp_connect LOOP "127.0.0.1" 8080 ON_CONNECT ON_READ ON_WRITE) 

	)

(loop

    (while (SDL_PollEvent)
        (let ( EVENT  @)

            (prinl "TYPE = " (car EVENT) ", EVENT = " (cdr EVENT) " CLOSE = " SDL_WINDOWEVENT_CLOSE)
            (prinl "TYPE = " (car EVENT) ", EVENT = " (cdr EVENT) " MOUSE = " SDL_MOUSEBUTTONDOWN)

        	(if (COMP_PACK (car EVENT) (PACK SDL_MOUSEBUTTONDOWN))
        	    (do 1
        	    (uv_queue_work LOOP TASK)
				(SDL_SetRenderDrawColor R 255 255 0)
				(let P (SDL_GetMouseState)
				(SDL_RenderDrawLine R 10 10 (car P) (cdr P)))
				(SDL_RenderPresent R)))

        	(if (COMP_PACK (car EVENT) (PACK SDL_USEREVENT))
        	    (do 1
        	    	(prinl "GOT THE USER EVENT" (car (cdr EVENT)));
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

