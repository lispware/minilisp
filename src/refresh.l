(prinl "RELOAD START")

(setq WINDOW_HEIGHT 800)
(setq WINDOW_WIDTH 1200)

(setq BORDER 10)
(setq START BORDER)
(setq LINE_WIDTH 5)
(setq STOP 550)
(setq JUMP 50)
(setq RAD 100)

(if RESIZE_DONE Nil
(SDL_SetWindowSize WIN WINDOW_WIDTH WINDOW_HEIGHT)
(setq RESIZE_DONE T))

(SDL_SetRenderDrawColor RENDERER 127 127 255 255)
(SDL_RenderFillRect RENDERER 0 0 WINDOW_WIDTH WINDOW_HEIGHT)

(SDL_SetRenderDrawColor RENDERER 200 0 0 200)

(de line (X1 Y1 X2 Y2)
	(for J 5
		(if (= X1 X2)
			(SDL_RenderDrawLine RENDERER  (+ J X1) Y1 (+ X2 J)  Y2)
			(SDL_RenderDrawLine RENDERER  X1 (+ J Y1) X2  (+ J Y2)))))
	
(for I 21
	(let (X1 (+ 10 (* I JUMP) )
		  Y1 START
		  X2 (+ 10 (* I JUMP) )
		  Y2 STOP)
	(line X1 Y1 X2 Y2)))

(for I 10
	(let (X1 (+ 10 (* I JUMP) )
		  Y1 START
		  X2 (+ 10 (* I JUMP) )
		  Y2 (+ 200 STOP))
	(line Y1 X1  Y2 X2)))

(SDL_SetRenderDrawColor RENDERER 255 255 255)


(de wheel (X Y)
    (for I 10
        (circleBres X Y (- RAD (- (* I 10) 1)))
        (circleBres X Y (- RAD (- (* I 10) 2)))
        (circleBres X Y (- RAD (* I 10)))))

(wheel 200 150)
(wheel 600 150)
(wheel 200 450)
(wheel 600 450)

(SDL_RenderPresent RENDERER)

(prinl "RELOAD END")

