(prinl "RELOAD START")

(setq START 50)
(setq STOP 550)
(setq JUMP 50)

(SDL_SetRenderDrawColor RENDERER 127 127 127 255)
(SDL_RenderFillRect RENDERER 0 0 800 600)

(SDL_SetRenderDrawColor RENDERER 200 0 0 255)


(SDL_RenderDrawLine RENDERER (+ 10 (* I 10) ) START (+ 10 (* I 10) ) STOP)


(de line (X1 Y1 X2 Y2)
	(for J 5
		(if (= X1 X2)
			(SDL_RenderDrawLine RENDERER  (+ J X1) Y1 (+ X2 J)  Y2)
			(SDL_RenderDrawLine RENDERER  X1 (+ J Y1) X2  (+ J Y2)))))
	

(for I 14
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

(SDL_RenderPresent RENDERER)

(prinl "RELOAD END")
