(de reverse (L)
    (let R Nil
        (for I L
            (setq R (cons I R)))
    R))
            
(loop 
    (while (sdlpoll)
        (let X (sdlistextinput)
            (if X (prinl (setq R (cons (char X) R))))
        (if (sdisenterpressed) (prinl "ENTER PRESSED: " (pack (reverse R))))
        (if (sdlisclose) (sdlclose) ))))
