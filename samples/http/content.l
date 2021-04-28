(de LEN (X)
    (setq C 0)
    (for i (chop X)
         (setq C (+ 1 C))))

(setq CONTENT "<html><body><h1>HELLO HTTP</h1><hr></body></html>")
(setq CONTENT-LENGTH (LEN CONTENT))
