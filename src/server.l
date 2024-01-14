(de server (TCP DATA)
	(prinl "HELLO"))


(setq LOOP (uv_loop))
(uv_tcp_listen LOOP "0.0.0.0" 8080 TCP DATA (handle TCP DATA))

(loop

    	(uv_run_nowait LOOP)
    	)
