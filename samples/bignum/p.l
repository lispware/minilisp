(setq X 100)
(setq Y (** 10 X))
(setq BYTES (out "file" (wr Y)))
# BYTES has the number of bytes written to file
(setq Z (in "file" (rd BYTES)))

(prinl "Wrote : " Y)
(prinl "Read  : " Z)

(bye)
