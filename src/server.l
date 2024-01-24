(de getColor (BUF)
	(let (A (chop BUF)
	      B (split A ",")
	      C (mapcar pack B)
	      D (mapcar format C)
	      X (car D)
	      Y (car (cdr D))
	      )

	(if (and (> X 400) (> Y 400))
	    "255,0,0"
		(if (> X 400)
		    "255,255,0"
		    (if (> Y 400)
		        "0,255,0"
		        "0,0,255")))))

(de ON_READ (BUF TCP_AND_DATA)
    (let (TCP (car TCP_AND_DATA)
          DD (car (cdr TCP_AND_DATA)))
		(uv_tcp_write TCP (getColor BUF) DATA (ON_WRITE TCP DATA))
	)
)

(de ON_WRITE (TCP DATA)
    (uv_close TCP)
	(prinl "WRITING DONE"))

(de handle (TCP DATA)
	(let TCP_AND_DATA (list TCP DATA)
	(uv_read_start TCP BUF TCP_AND_DATA (ON_READ BUF TCP_AND_DATA)))
	(prinl "HELLO READ STARTED"))


(setq LOOP (uv_loop))
(let DATA 100
(uv_tcp_listen LOOP "0.0.0.0" 8080 TCP DATA (handle TCP DATA)))

(loop (uv_run_nowait LOOP))
