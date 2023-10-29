# uv_tcp_t = 248
# addr 16
# struct sockaddr_in 16
# sizeof(write_req_t) = 208
# sizeof(uv_tcp_t) = 248

(setq LOOP (uv_default_loop))


(de RD (CLIENT BUF NREAD )
	(prinl "RD CLIENT=" CLIENT  " BUF=" BUF " NREAD=" NREAD)
	(uv_write CLIENT BUF NREAD)

	)

(de bingo (SERVER STATUS)

	(let (CLIENT (A (+ 208 100)))

		(uv_tcp_init LOOP CLIENT)
		(prinl "ABOUT TO ACCEPT")
		(if (uv_accept SERVER CLIENT RD)
			(prinl "ACCEPT SUCCESSFUL")
			(prinl "ACCEPT FAILED"))
	)

	(prinl "ON CONNECTION STATUS = " SERVER " " STATUS)
	)



(setq TCP (A (+ 248 100)))

(uv_tcp_init LOOP TCP)

# uv_ip4_addr("0.0.0.0", DEFAULT_PORT, &addr);
(setq ADDR (A 16))
(uv_ip4_addr "0.0.0.0" 7000 ADDR)

(uv_tcp_bind TCP ADDR)


(uv_listen TCP bingo)

(uv_run LOOP)
