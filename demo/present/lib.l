(de g-clear ()
	(SDL_RenderClear RENDERER))

(de g-color (R G B A)
   (SDL_SetRenderDrawColor RENDERER R G B A) )

(de drawCircle (XC YC X Y)
   (SDL_RenderDrawPoint RENDERER (+ XC X) (+ YC Y))
   (SDL_RenderDrawPoint RENDERER (- XC X) (+ YC Y))
   (SDL_RenderDrawPoint RENDERER (+ XC X) (- YC Y))
   (SDL_RenderDrawPoint RENDERER (- XC X) (- YC Y))
   (SDL_RenderDrawPoint RENDERER (+ XC Y) (+ YC X))
   (SDL_RenderDrawPoint RENDERER (- XC Y) (+ YC X))
   (SDL_RenderDrawPoint RENDERER (+ XC Y) (- YC X))
   (SDL_RenderDrawPoint RENDERER (- XC Y) (- YC X)) )

(de g-circle (XC YC R)
   (let (X 0  Y R  D (- 3 (* 2 R)))
      (drawCircle XC YC X Y)
      (while (or (> Y X) (= Y X))
         (setq X (+ 1 X))
         (if (> D 0)
            (do 1
               (setq Y (- Y 1))
               (setq D
                  (+ D (* 4 (- X Y)) 10) ) )
            (setq D (+ D (* 4 X) 6)) )
         (drawCircle XC YC X Y) ) ) )

(de g-line (X1 Y1 X2 Y2)
   (SDL_RenderDrawLine RENDERER X1 Y1 X2 Y2) )

(de g-draw-char (C XX YY RW RH)
   (let
      (N (char C)
         I (- N 32)
         D (/ I 15)
         R (% I 15)
         Y (+ FONT_OFFSET_Y (* D FONT_HEIGHT))
         X (+ FONT_OFFSET_X (* R FONT_WIDTH 2)) )
      (SDL_RenderCopy RENDERER FONT_TEXTURE X Y FONT_WIDTH FONT_HEIGHT XX YY RW RH) ) )

(de g-draw-str (S XX YY RW RH)
   (let X XX
      (for C (chop S)
         (draw-char C X YY RW RH)
         (setq X (+ X RW)) ) ) )
