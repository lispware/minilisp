(prinl "RELOAD START")

(setq WINDOW_WIDTH 1000)
(setq WINDOW_HEIGHT 1000)

(setq BORDER 50)
(setq STOP 550)
(setq JUMP 50)
(setq RAD 50)

(SDL_SetRenderDrawColor RENDERER 0 0 0 255)
(SDL_RenderClear RENDERER)

################################################################################
(de g-color (R G B A)
	(SDL_SetRenderDrawColor RENDERER R G B A))

(de drawCircle (XC YC X Y)
 	(SDL_RenderDrawPoint RENDERER (+ XC X) (+ YC Y))
 	(SDL_RenderDrawPoint RENDERER (- XC X) (+ YC Y))
 	(SDL_RenderDrawPoint RENDERER (+ XC X) (- YC Y))
 	(SDL_RenderDrawPoint RENDERER (- XC X) (- YC Y))
 	(SDL_RenderDrawPoint RENDERER (+ XC Y) (+ YC X))
 	(SDL_RenderDrawPoint RENDERER (- XC Y) (+ YC X))
 	(SDL_RenderDrawPoint RENDERER (+ XC Y) (- YC X))
 	(SDL_RenderDrawPoint RENDERER (- XC Y) (- YC X))
 	)

(de g-circle (XC YC R)
    (let (X 0
          Y R
          D (- 3 (* 2 R))
          )
	 (drawCircle XC YC X Y)
    (while (or (> Y X) (= Y X))
           (setq X (+ 1 X))
           (if (> D 0)
               (do 1 (setq Y (- Y 1)) (setq D (+ D (* 4 (- X Y)) 10)))
               (setq D (+ D (* 4 X) 6))
               )
           (drawCircle XC YC X Y)
           )
    ))

(de g-line (X1 Y1 X2 Y2)
			(SDL_RenderDrawLine RENDERER  X1 Y1 X2  Y2))
################################################################################
	

# (for I 15
# 	(let (Y1 (+ BORDER (* (- I 1) JUMP) )
# 		  X1 BORDER
# 		  Y2 Y1
# 		  X2 (- WINDOW_WIDTH BORDER))
# 	(g-line X1 Y1 X2 Y2)))
# 
# (de wheel (X Y)
#     (for I 4
# 		(SDL_SetRenderDrawColor RENDERER 255 (* I 50) (* I 50))
#         (g-circle X Y (- RAD (- (* I 10) 1)))
#         (g-circle X Y (- RAD (- (* I 10) 2)))
#         (g-circle X Y (- RAD (* I 10)))))
# 
# (for Y 4
# 	(for X 5
# 		(wheel (+ 100 (* (- X 1) 250)) (+ 100 (* (- Y 1) 200)))))
# 

(SDL_RenderPresent RENDERER)

(prinl "RELOAD END")

#(setq IMG (IMG_Load "RGB.bmp"))
#(SDL_SetTransparency WIN IMG 0 0 0)
#(setq TEXTURE (SDL_CreateTextureFromSurface RENDERER IMG))
#(SDL_SetRenderDrawColor RENDERER 255 255 255 100)
#(SDL_RenderFillRect RENDERER 0 0 WINDOW_WIDTH WINDOW_HEIGHT)
#(SDL_SetTextureBlendMode TEXTURE 1)
#(SDL_SetTextureAlphaMod TEXTURE 127)
#(SDL_RenderCopy RENDERER TEXTURE 0 0 200 200 200 200 200 200)
#(SDL_RenderPresent RENDERER)

(SDL_SetTextureBlendMode FONT_TEXTURE 1)


(de render-font ()
	(SDL_RenderCopy RENDERER FONT_TEXTURE 0 0 2231 1111 0 0 2231 1111)
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
			(SDL_RenderFillRect RENDERER (+ SX  (* (- C 1) W)) (+ SY (* (- R 1) H)) W H))))))
# (render-font) # Call this if you need to see the font bmp file rendered

(de highlight (C)
        (let (N (char C)
              I (- N 32)
              D (/ I 15)
              R (% I 15)
              Y (+ FONT_OFFSET_Y (* D FONT_HEIGHT))
              X (+ FONT_OFFSET_X (* R FONT_WIDTH 2))
              )
	    (SDL_SetRenderDrawColor RENDERER 127 0 0 100)
		(SDL_RenderFillRect RENDERER X Y FONT_WIDTH FONT_HEIGHT)
))

(de draw-char (C XX YY RW RH)
        (let (N (char C)
              I (- N 32)
              D (/ I 15)
              R (% I 15)
              Y (+ FONT_OFFSET_Y (* D FONT_HEIGHT))
              X (+ FONT_OFFSET_X (* R FONT_WIDTH 2))
              )
        (SDL_RenderCopy RENDERER FONT_TEXTURE X Y FONT_WIDTH FONT_HEIGHT XX YY RW RH)
))

(de draw-str (S XX YY RW RH)
    (let X XX
		(for C (chop S)
			(draw-char C X YY RW RH)
			(setq X (+ X RW)))))
	
(let B 10
	(for I (/ WINDOW_WIDTH B)
		(if (not (= 0 (% I 10)) )
			(g-color 50 50 127 127)
			(g-color 100 100 200 127))
		(let X (* I B)
			(g-line X 0 X (- WINDOW_HEIGHT 1)))))
(let B 10
	(for I (/ WINDOW_HEIGHT B)
		(if (not (= 0 (% I 10)) )
			(g-color 50 50 127 127)
			(g-color 100 100 200 127))
		(let Y (* I B)
			(g-line 0 Y (- WINDOW_WIDTH 1) Y))))

(let D 1
	(draw-str "PicoLisp is awesome!" 100 100 (/ FONT_WIDTH D) (/ FONT_HEIGHT D)))

(g-color 100 255 0 255)
(g-circle 500 500 200)
(g-line 600 300 340 650)
(g-line 450 450 660 650)
		
(SDL_RenderPresent RENDERER)
