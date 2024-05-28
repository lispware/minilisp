(setq LOOP (uv_loop))


(setq TIMER (uv_timer_init LOOP))


(setq X 20)

(de callback (A B C D E F G)
    (setq X (- X 1))
    (if (= X 10) (uv_timer_stop TIMER))
    (prinl "CALLBACK CALLED " A " " B " " C " " D " " E " " F " " G " "X))

(prinl "--------------------------")
(uv_timer_start TIMER 1000 10 callback)
(setq X 30)

(gc)

(loop
    (uv_run_nowait LOOP))    
