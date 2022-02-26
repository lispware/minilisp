(setq IS_WINDOWS (= "Windows" (os)))

# read a file till end
(de slurp (F)
    (in F
        (pack
            (make (let Y (rd 1)
                (while (car (cdr Y))
                    (link (char (car Y)))
                    (setq Y (rd 1))))))))


