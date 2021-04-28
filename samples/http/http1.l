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

    (sktClose *S)

)

(setq *B (bind 8080))
(loop
   (setq *S (listen *B))
   (fork (http *S (ECHO *S)))
)
