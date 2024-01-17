(de ON_READ (BUF DINGO)
	(prinl "ON_READ")
	(prinl BUF)

	)

(de ON_WRITE (TCP DATA)
	(prinl "WRITING DONE"))

(de handle (TCP DATA)
	(let DINGO 200
	(uv_read_start TCP BUF DINGO (ON_READ BUF DINGO)))
	(uv_tcp_write TCP  "APPLE MANGO BANANA" DATA (ON_WRITE TCP DATA))
	(prinl "HELLO 123"))


(setq LOOP (uv_loop))
(let DATA 100
(uv_tcp_listen LOOP "0.0.0.0" 8080 TCP DATA (handle TCP DATA)))

(loop

    	(uv_run_nowait LOOP)
    	)
