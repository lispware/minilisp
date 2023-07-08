(setq X 100)
(setq Y (+ 12345678 (** 10 X)))
(setq BYTES (out "file" (wr Y)))
# BYTES has the number of bytes written to file
(setq Z (in "file" (rd BYTES)))

(if (car (cdr Z))
    (do 1
        (prinl "Read was successful")
        (prinl "Wrote : " Y)
        (prinl "Read  : " (car Z)))
    (prinl "READ failed"))

(bye)
