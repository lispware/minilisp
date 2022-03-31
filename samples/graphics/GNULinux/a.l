(de reverse (L)
    (let R Nil
        (for I L
            (setq R (cons I R)))
    R))


(de box (X Y S)
    (for I S
        (sdlputpixel (- (+ X I) 1)  Y 255 255 255)
        (sdlputpixel (- (+ X I) 1) (+ Y S) 255 255 255)
        
        (sdlputpixel X (- (+ Y I) 1) 255 255 255)
        (sdlputpixel (+ X S) (- (+ Y I) 1) 255 255 255))
    (sdlupdatewindow)
    )
           
(loop 
    (while (sdlpoll)
        (let X (sdlistextinput)
            (if X (prinl (reverse (setq R (cons (char X) R)))) )


        (if (sdisbackspacepressed) (do 1 (setq R (cdr R)) (prinl (reverse R))))

        (if (sdisenterpressed) (do 1 (prinl "ENTER PRESSED: "  (load (pack (reverse R)))) (setq R Nil) ))
        (if (sdlisclose) (sdlclose) ))))
