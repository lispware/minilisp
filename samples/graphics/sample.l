(setq HEIGHT 1000)
(setq WIDTH 1000)

(sdlCreateWindow "HELLO" WIDTH HEIGHT 0 0 0)

(setq SPACES "                                                                                ")

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

(de box (X Y S R1 G1 B1)
    (let (CF '((C) (if C C 255))
          R (CF R1)
          G (CF G1)
          B (CF B1))
        (sdlDrawLine X Y (+ X S) Y  R G B)
        (sdlDrawLine X Y X (+ Y S)  R G B)
        (sdlDrawLine X (+ Y S) (+ X S) (+ Y S)  R G B)
        (sdlDrawLine (+ X S) (+ Y S) (+ X S) Y  R G B)))

(de clear (R G B)
    (let CF '((C) (if C C 255))
     (sdlClearWindow (CF R) (CF G) (CF B)))
    (sdlPresentRenderer))

(de update () (sdlPresentRenderer))

(de demo ()
    (box 100 100 200 255 0 0)
    (sdlPresentRenderer)
    (sdlDelay 200)
    (box 100 100 200 0 255 0)
    (sdlPresentRenderer)
    (sdlDelay 200)
    (box 100 100 200 0 0 255)
    (sdlPresentRenderer)
    (sdlDelay 200)
    )

(de write (X Y S)
    (sdlWriteString X Y 255 255 255 0 0 255 2 S))


(de updateLine (X)
    (write 10 800 SPACES)
    (if X (write 10 800 (pack X)))
    (sdlPresentRenderer))

(write 10 800 SPACES)

(loop
    (while (sdlPoll)
        (let X (sdlIsTextInput)
            (if X (updateLine (reverse (setq R (cons (char X) R)))) )


        (if (sdIsBackspacePressed) (do 1 (setq R (cdr R)) (updateLine (reverse R))))

        (if (sdIsEnterPressed) (do 1 (if R (prinl "ENTER PRESSED: "  (load (pack (reverse R))))) (setq R Nil) (sdlPresentRenderer) ))
        (if (sdlIsClose) (sdlClose) ))))
