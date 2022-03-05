(setq PASS_COUNT 0)
(setq FAIL_COUNT 0)

(de TEST (N Expected Actual)
    (let (V (= Expected Actual)
          M (if (not V) (pack N " => Expected: " Expected "; but got: " Actual)))
        (if V (setq PASS_COUNT (+ PASS_COUNT 1)) (setq FAIL_COUNT (+ FAIL_COUNT 1)))
        (prinl (if V "PASSED" "FAILED") ": " (if V N M))))

(load "lib.l")

(de LISP  @ 
    (let (CMD  (if IS_WINDOS "lisp.exe" "./lisp")
          CMD_AND_ARGS (pack (make (link CMD) (while (args) (link " ") (link (next))))))
        CMD_AND_ARGS))

(let CMD (LISP "'(out \"TESTFILE\" (prinl \"HELLO WORLD\"))'" "-bye") 
    (out "TESTFILE")
    (call CMD)
    (TEST CMD (pack "HELLO WORLD" (char 10)) (slurp "TESTFILE")))

(let CMD (LISP "'(do 1 (out \"TESTFILE\" (prinl (argv))) (bye))'" "-" 1 2 "ABCD") 
    (out "TESTFILE")
    (call CMD)
    (TEST CMD (pack "12ABCD" (char 10)) (slurp "TESTFILE")))

(hs)
(prinl "PASS_COUNT = " PASS_COUNT ", FAIL_COUNT = " FAIL_COUNT);
(bye)
