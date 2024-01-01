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


(loop

    (while (SDL_PollEvent)
        (let ( EVENT  @)

            (prinl "TYPE = " (car EVENT) ", EVENT = " (cdr EVENT) " CLOSE = " SDL_WINDOWEVENT_CLOSE)

        	(if (COMP_PACK (car (cdr EVENT)) (PACK SDL_WINDOWEVENT_CLOSE))
        		(prinl "IT IS CLOSE")
        		(prinl "IT IS NOT CLOSE"))

        	(if (COMP_PACK (car (cdr EVENT)) (PACK SDL_WINDOWEVENT_CLOSE))
        		(do 1 (SDL_DestroyWindow W) (SDL_Quit) (bye)))


            )
        )
    )

