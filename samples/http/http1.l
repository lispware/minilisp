# This sample illustrates a simple HTTP server

(de SKIP-REQUEST-HEADER ()
    (let X 1
          (while (not (= X ""))
            (setq X (pack (line)))
          )
    )
)

(de PRINT-RESPONSE-HEADER ()
    (prinl "HTTP/1.0 200 OK")
    (prinl "Content-Length: " CONTENT-LENGTH)
    (prinl)
)

(de ECHO (*S)
    (load "content.l")

    (SKIP-REQUEST-HEADER)

    (PRINT-RESPONSE-HEADER)

    (prinl CONTENT)

)

(setq *B (bind 8080))
(loop
   (setq *S (listen *B))

    # means, zero out  *S from the calling thread so that it is not GC'd from both threads
   (thread '(*S) (socket *S (ECHO *S))) 
)
