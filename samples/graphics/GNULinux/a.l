(de reverse (L)
    (let R Nil
        (for I L
            (setq R (cons I R)))
    R))

           
(loop 
    (while (sdlpoll)
        (let X (sdlistextinput)
            (if X (prinl (reverse (setq R (cons (char X) R)))) )


        (if (sdisbackspacepressed) (do 1 (setq R (cdr R)) (prinl (reverse R))))

        (if (sdisenterpressed) (do 1 (prinl "ENTER PRESSED: "  (load (pack (reverse R)))) (setq R Nil) ))
        (if (sdlisclose) (sdlclose) ))))
