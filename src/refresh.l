(prinl "RELOAD START")

(setq WINDOW_WIDTH 800)
(setq WINDOW_HEIGHT 600)


(setq BORDER 50)
(setq START BORDER)
(setq LINE_WIDTH 2)
(setq STOP 550)
(setq JUMP 50)
(setq RAD 50)

(if RESIZE_DONE Nil
(SDL_SetWindowSize WIN WINDOW_WIDTH WINDOW_HEIGHT)
(setq RESIZE_DONE T))

(SDL_SetRenderDrawColor RENDERER 120 255 120 255)
(SDL_RenderFillRect RENDERER 0 0 WINDOW_WIDTH WINDOW_HEIGHT)

(SDL_SetRenderDrawColor RENDERER 100 0 0 200)

(de line (X1 Y1 X2 Y2)
	(for J LINE_WIDTH
		(if (= Y1 Y2)
			(SDL_RenderDrawLine RENDERER  X1 (+ Y1 J -1) X2 (+ Y2 J -1))
			(SDL_RenderDrawLine RENDERER  (+ X1 J -1) Y1 (+ X2 J -1)  Y2))))
	
(for I 23
	(let (X1 (+ BORDER (* (- I 1) JUMP) )
		  Y1 BORDER
		  X2 X1
		  Y2 (- WINDOW_HEIGHT BORDER))
	(line X1 Y1 X2 Y2)))

(for I 15
	(let (Y1 (+ BORDER (* (- I 1) JUMP) )
		  X1 BORDER
		  Y2 Y1
		  X2 (- WINDOW_WIDTH BORDER))
	(line X1 Y1 X2 Y2)))



(de wheel (X Y)
    (for I 4
		(SDL_SetRenderDrawColor RENDERER 255 (* I 50) (* I 50))
        (circleBres X Y (- RAD (- (* I 10) 1)))
        (circleBres X Y (- RAD (- (* I 10) 2)))
        (circleBres X Y (- RAD (* I 10)))))

(for Y 4
	(for X 5
		(wheel (+ 100 (* (- X 1) 250)) (+ 100 (* (- Y 1) 200)))))


#(SDL_RenderClear RENDERER)
(SDL_RenderPresent RENDERER)

(prinl "RELOAD END")

(setq IMG (IMG_Load "DINGO"))
(setq TEXTURE (SDL_CreateTextureFromSurface RENDERER IMG))
(SDL_SetRenderDrawColor RENDERER 255 255 255 255)
(SDL_RenderCopy RENDERER TEXTURE)
(SDL_RenderPresent RENDERER)
