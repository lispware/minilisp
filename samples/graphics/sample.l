(sdlCreateWindow "HELLO" 2000 1000 255 0 0)

(de reverse (L)
    (let R Nil
        (for I L
            (setq R (cons I R)))
    R))


(de putPixel (X Y R G B)
    (sdlDrawLine  X Y X Y R G B))

(de line (X1 Y1 X2 Y2 R G B)
    (let CF '((C) (if C C 255))
        (sdlDrawLine X1 Y1 X2 Y2 (CF R) (CF G) (CF B))))

(de box (X Y S R G B)
    (sdlDrawLine X Y (+ X S) Y  R G B)
    (sdlDrawLine X Y X (+ Y S)  R G B)
    (sdlDrawLine X (+ Y S) (+ X S) (+ Y S)  R G B)
    (sdlDrawLine (+ X S) (+ Y S) (+ X S) Y  R G B))

(de clear (R G B)
    (sdlClearWindow R G B)
    (sdlPresentRenderer))

(loop
    (while (sdlPoll)
        (let X (sdlIsTextInput)
            (if X (prinl (reverse (setq R (cons (char X) R)))) )


        (if (sdIsBackspacePressed) (do 1 (setq R (cdr R)) (prinl (reverse R))))

        (if (sdIsEnterPressed) (do 1 (if R (prinl "ENTER PRESSED: "  (load (pack (reverse R))))) (setq R Nil) ))
        (if (sdlIsClose) (sdlClose) ))))
