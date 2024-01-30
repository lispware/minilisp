(prinl "RELOAD START")

(setq WINDOW_WIDTH 2300)
(setq WINDOW_HEIGHT 1200)

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

(setq IMG (IMG_Load "RGB.bmp"))
(SDL_SetTransparency WIN IMG 0 0 0)
(setq TEXTURE (SDL_CreateTextureFromSurface RENDERER IMG))
(SDL_SetRenderDrawColor RENDERER 255 255 255 100)
(SDL_RenderFillRect RENDERER 0 0 WINDOW_WIDTH WINDOW_HEIGHT)
(SDL_SetTextureBlendMode TEXTURE 1)
(SDL_SetTextureAlphaMod TEXTURE 127)
(SDL_RenderCopy RENDERER TEXTURE 0 0 200 200 200 200 200 200)
(SDL_RenderPresent RENDERER)

(SDL_SetRenderDrawColor RENDERER 0 0 255 200)
(SDL_RenderFillRect RENDERER 0 0 WINDOW_WIDTH WINDOW_HEIGHT)
(setq FONT (IMG_Load "font_72x144_62x38.bmp"))
(SDL_SetTransparency WIN FONT 0 0 0)
(setq FONT_TEXTURE (SDL_CreateTextureFromSurface RENDERER FONT))
(SDL_SetTextureBlendMode FONT_TEXTURE 1)
(SDL_RenderCopy RENDERER FONT_TEXTURE 0 0 2231 1111 0 0 2231 1111)


(setq FONT_OFFSET_X 62)
(setq FONT_OFFSET_Y 38)
(setq FONT_WIDTH 72)
(setq FONT_HEIGHT 144)


(let (SX FONT_OFFSET_X
      SY FONT_OFFSET_Y
      W FONT_WIDTH
      H FONT_HEIGHT)
(for R 7
	(if ALT (setq ALT Nil) (setq ALT 1))
	(if ALT (setq ALT Nil) (setq ALT 1))
	(for C 29 
	    (do 1 
	    (if ALT (setq ALT Nil) (setq ALT 1))
	    (SDL_SetRenderDrawColor RENDERER 255 255 (if ALT 255 0) 200)
		(SDL_RenderFillRect RENDERER (+ SX  (* (- C 1) W)) (+ SY (* (- R 1) H)) W H)))))


(de highlight (C)
        (let (N (char C)
              I (- N 32)
              D (/ I 15)
              R (% I 15)
              Y (+ FONT_OFFSET_Y (* D FONT_HEIGHT))
              X (+ FONT_OFFSET_X (* R FONT_WIDTH 2))
              )
        (prinl R)
	    (SDL_SetRenderDrawColor RENDERER 127 0 0 100)
		(SDL_RenderFillRect RENDERER X Y 72 145)
))

(de draw-char (C XX YY RW RH)
        (let (N (char C)
              I (- N 32)
              D (/ I 15)
              R (% I 15)
              Y (+ FONT_OFFSET_Y (* D FONT_HEIGHT))
              X (+ FONT_OFFSET_X (* R FONT_WIDTH 2))
              )
        (SDL_RenderCopy RENDERER FONT_TEXTURE X Y 72 145 XX YY RW RH)
))

(de draw-str (S XX YY RW RH)
    (let X XX
		(for C (chop S)
			(draw-char C X YY RW RH)
			(setq X (+ X RW)))))
	

(highlight "c")
(let D 1
(draw-str "PicoLisp is awesome!" 100 1050 (/ FONT_WIDTH D) (/ FONT_HEIGHT D)))
		
(SDL_RenderPresent RENDERER)
