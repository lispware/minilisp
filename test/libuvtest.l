(setq LOOP (uv_loop))

(de exists (E)
		(if E (do 1 (prinl "TEST PASSED") (bye))
        	  (prinl "TEST FAILED: EXISTS")
        	  (bye)))

(de does-not-exist (E)
		(if E (do 1 (prinl "TEST FAILED: DOES NOT EXIST") (bye)))
		(uv_fs_stat LOOP "libuvtest.l" E (exists E)))


(uv_fs_stat LOOP "DoesNotExist" E (does-not-exist E))

(loop 
(uv_run_nowait LOOP) )
