(setq LOOP (uv_loop))

(setq X 20)
(setq TIMER (uv_timer_init LOOP))

(setq CONTENT 0)
(setq XX 1234)

(de on_write (F B L)
    (prinl "F = " F)
    (prinl "B = " B)
    (prinl "L = " L)
    (prinl "ON WRITE CALLED ANTARCTICA")
)

(de writeprocess (FILE)
    (prinl "XX <" XX  "> ")
    (prinl "CONTENT2  = <" (buf2str CONTENT) "> ")
    (uv_fs_write LOOP FILE CONTENT 2 (on_write F B L))
    (uv_fs_write LOOP FILE CONTENT 2 (on_write F B L))
    (uv_fs_write LOOP FILE (str2buf "HELLO WORLD") 11 (on_write F B L))
)

(de on_read (F BUFFER LEN)
    (prinl)
    (prinl)
    (prinl)
    (prinl "BUFFER SIZE = " LEN)
    (prinl "BUFFER  = <" (buf2str BUFFER) "> ")

    (setq CONTENT BUFFER)
    (setq XX "ABCD")
    (prinl "CONTENT  = <" (buf2str CONTENT) "> ")
    #(uv_fs_open LOOP "dingo" WRITE (writeprocess X))
    #(gc)

    )

(setq FILE 0)

(de callback (A B C D E F G)
    (setq X (- X 1))
    (if (= X 15)
        (do 1 (uv_timer_stop TIMER) (uv_fs_close LOOP FILE))
        (uv_fs_read LOOP FILE 20 on_read))

    (prinl "CALLBACK CALLED " A " " B " " C " " D " " E " " F " " G " " X))

(de process (F)
    (let Y 20
        (prinl F)
        (setq FILE F)
        (uv_timer_start TIMER 1000 10 callback)
        (prinl "PROCESS CALLED " X)))


(uv_fs_open LOOP "a.l" READ process)

(loop
    (uv_run_nowait LOOP))    
