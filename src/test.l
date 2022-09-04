(setq PASS_COUNT 0)
(setq FAIL_COUNT 0)

(de TEST (N Expected Actual)
    (let (V (= Expected Actual)
          M (if (not V) (pack N " => Expected: " Expected "; but got: " Actual)))
        (if V (setq PASS_COUNT (+ PASS_COUNT 1)) (setq FAIL_COUNT (+ FAIL_COUNT 1)))
        (prinl (if V "PASSED" "FAILED") ": " (if V N M))))

################################################################################
# quote
################################################################################
(TEST "quote double quoted string" '("hello") (eval '(quote "hello")))
(TEST "quote symbol" '(hello) (eval '(quote "hello")))
(TEST "quote numbers" (1 2 3) (eval '(quote 1 2 3)))
(TEST "quote symbols" 3 (let (A 1 B 2) (eval (quote + A B))))

################################################################################
# de
################################################################################
(de F () 10)
(TEST "de - void -> num" 10 (F))

(de F () "ABCD")
(TEST "de - void -> str" "ABCD" (F))

(de F (X Y) 
    (let A 10 
         (let (AA 20 BB 30)
        (+ A AA BB X Y))))
(TEST "de with let" 90 (F 10 20))

(de V A (make (for I A (link I))))
(TEST "de variable number of arguments" '(1 2 3 "A" "B" "C") (V 1 2 3 "A" "B" "C"))
(TEST "de variable number of arguments" (1 2 3 'A 'B 'C) (V 1 2 3 'A 'B 'C))
(TEST "de variable number of arguments" '(1 2 3 'A 'B 'C) (V 1 2 3 'A 'B 'C))

(de V @ (make (while (args) (link (next)))))
(TEST "de variable number of arguments using @" '(1 2 3 A B C) (V 1 2 3 'A 'B 'C))

################################################################################
# math
################################################################################

(TEST "(+)" 0 (+))
(TEST "(+ 1)" 1 (+ 1))
(TEST "(+ 1 2)" 3 (+ 1 2))
(TEST "(+ A B C) # A=1, B=2, C=3" 6 (let (A 1 B 2 C 3) (+ A B C)))
(let N 233423423432426465767456323343 (TEST (pack "(+ " N  " 1)") 233423423432426465767456323344 (+ N 1)))

(TEST "(-)" 0 (-))
(TEST "(- 1)" 1 (- 1)) # this is different from PIL
(TEST "(- 2 1)" 1 (- 2 1))
(TEST "(- A B C) # A=10, B=2, C=3" 5 (let (A 10 B 2 C 3) (- A B C)))
(let N 233423423432426465767456323343 (TEST (pack "(- " N  " 1)") 233423423432426465767456323342 (- N 1)))

(TEST "(*)" 1 (*))
(TEST "(* 1)" 1 (* 1)) # this is different from PIL
(TEST "(* 2 3)" 6 (* 2 3))
(TEST "(* A B C) # A=10, B=2, C=3" 60 (let (A 10 B 2 C 3) (* A B C)))
(let N 233423423432426465767456323343 (TEST (pack "(* " N  " (+ " N " 1))") 54486494606913860857456602291802225080642579694922823018992 (* N (+ N 1))))

(TEST "(/)" Nil (/))
(TEST "(/ 1)" Nil (/ 1))
(TEST "(/ 10 2)" (5 0) (/ 10 2))
(TEST "(/ 10 2 2)" (5 0) (/ 10 2 2))
(TEST "(/ A B) # A=10, B=2" (5 0) (let (A 10 B 2) (/ A B)))

(TEST "(%)" Nil (%))
(TEST "(% 1)" Nil (% 1))
(TEST "(% 10 2)" 0 (% 10 2))
(TEST "(% 10 3)" 1 (% 10 3))
(TEST "(% 10 2 2)" 0 (% 10 2 2))
(TEST "(% A B) # A=27, B=7" 6 (let (A 27 B 7) (% A B)))

(let (N 100
      LS -1
      RS 1
      AFTER_LS (>> LS N)
      AFTER_LS_EXPECTED 200
      AFTER_RS (>> RS N)
      AFTER_RS_EXPECTED 50)
    (TEST "RIGHT SHIFT" AFTER_RS_EXPECTED AFTER_RS)
    (TEST "LEFT SHIFT" AFTER_LS_EXPECTED AFTER_LS))

(TEST "(& 1 1)" 1 (& 1 1))
(TEST "(| 1 2)" 3 (| 1 2))
(TEST "(x| 1 1)" 0 (x| 1 1))
(TEST "(** 2 10)" 1024 (** 2 10))
(TEST "(< 1 10)" T (< 1 10))
(TEST "(> 1 10)" Nil (> 1 10))
(TEST "(< 0 (let X 1 (rand X)))" T (< 0 (let X 1 (rand X))))
(TEST "(< 0 (rand X))" T (< 0 (rand X)))

(TEST "nested let" (11 10) (make (let X 10 (let X (+ 1 X) (link X)) (link X))))
(TEST "Single let binding" 10 (let X 10 X))
(TEST "multiple let bindings" 10 (let (X 5 Y 5) (+ X Y)))

(TEST "do 0" Nil (make (do 0 (link 1))))
(TEST "do 5" (1 1 1 1 1) (make (do 5 (link 1))))
(TEST "do 5 with quoted stuff" (cons 'a (cons 'a (cons 'a Nil))) (make (do 3 (link 'a))))

(setq N 10)
(TEST "(setq N 10)" 10 N)
(let X 10 (setq N (+ 1 X)))
(TEST "(let X 10 (setq N (+ 1 X)))" 11 N)
(setq F '((A B) (+ A B)))
(TEST "(setq F '((A B) (+ A B))) (let (X 10 Y 20) (F X Y))" 30 (let (X 10 Y 20) (F X Y)))

(TEST "(let X (1 2 3) (cons (++ X) (cons X Nil)))" (1 (2 3)) (let X (1 2 3) (cons (++ X) (cons X Nil))))
(TEST "(let X (1 2 3) (++ X))" 1 (let X (1 2 3) (++ X)))

################################################################################
# make/link/list/length
################################################################################
(TEST "(make)" Nil (make))
(TEST "(make (link 1))" (1) (make (link 1)))
(TEST "(make (link 1) (link 2))" (1 2) (make (link 1) (link 2)))
(TEST "(make (do 2 (link 1)))" (1 1) (make (do 2 (link 1))))
(TEST "(make (for I 2 (link I)))" (1 2) (make (for I 2 (link I))))
(TEST "(make (for I (1 2) (link I)))" (1 2) (make (for I (1 2) (link I))))
(TEST "(list)" (cons Nil Nil) (list))
(TEST "(list 1 2)" (1 2) (list 1 2))
(TEST "(list (+ 1 1) (+ 2 1))" (2 3) (list (+ 1 1) (+ 2 1)))
(TEST "(let (a 10 b 20) (list a b))" (10 20) (let (a 10 b 20) (list a b)))
(TEST "(length)" Nil (length))
(TEST "(length 100)" 100 (length 100))
(TEST "(length \"a\")" 1 (length "a"))
(TEST "(length \"abcd\")" 4 (length "abcd"))
(TEST "(length \"abcd\")" 4 (length "abcd"))
(TEST "(length (list 1 2 3))" 3 (length (list 1 2 3)))
(TEST "(length (list))" 1 (length (list)))

################################################################################
# index
################################################################################
(TEST "(index)" Nil (index))
(TEST "(index 1)" Nil (index 1))
(TEST "(index 1 (1 2 3))" 1 (index 1 (1 2 3)))
(TEST "(index 3 (1 2 3))" 3 (index 3 (1 2 3)))
(TEST '(index "a" ("b" "a" "c")) 2 (index "a" '("b" "a" "c")))

################################################################################
# cons/car/cdr
################################################################################
(TEST "(car)" Nil (car))
(TEST "(cdr)" Nil (cdr))
(TEST "(cons 1 Nil)" (1) (cons 1 Nil))
(TEST "(cons 1 (cons 2 Nil))" (1 2) (cons 1 (cons 2 Nil)))

################################################################################
# while
################################################################################
(setq X 3)
(TEST "while" (3 2 1) (make (while (> X 0) (link X) (setq X (- X 1)))))
    
################################################################################
# in/out char line rd wr
################################################################################
(out "TESTFILE")
(TEST "readback empty" "" (in "TESTFILE" (pack (line))))
(out "TESTFILE" (prinl "LINE1"))
(TEST "readback" "LINE1" (in "TESTFILE" (pack (line))))
(out "+TESTFILE" (prinl "LINE2"))
(TEST "readback after append" "LINE1LINE2" (in "TESTFILE" (pack (line) (line))))
(TEST "readback with char" "LINE1LINE2" (in "TESTFILE" (pack (make (do 5 (link (char))) (char) (do 5 (link (char)))))))

(TEST "(char 65)" "A" (char 65))

################################################################################
# sb - switch base for numbers
################################################################################
(TEST "(sb)" Nil (sb))
(TEST "(sb 10)" "A" (sb 10 16))
(TEST "(sb 10 16)" "A" (sb 10 16))
(TEST "(sb 10 2)" "1010" (sb 10 2))
(TEST "(sb \"123\" 10)" 123 (sb "123" 10))
(TEST "(sb \"A\" 16)" 10 (sb "A" 16))

################################################################################
# = not cmp and or
################################################################################
(TEST "(=)" T (=))
(TEST "(= 1 1)" T (= 1 1))
(TEST "(= 1 2)" Nil (= 1 2))
(TEST "(= \"a\" \"a\")" T (= "a" "a"))
(TEST "(= \"b\" \"a\")" Nil (= "b" "a"))
(TEST "(= \"aaaaaaaaaaaaaaaa\" (pack (make (do 16 (link \"a\")))))" T (= "aaaaaaaaaaaaaaaa" (pack (make (do 16 (link "a"))))))
(TEST "(= () ())" T (= () ()))
(TEST "(= (1 2 3) (1 2 3))" T (= (1 2 3) (1 2 3)))
(TEST "(= (1 2) (1 2 3))" Nil (= (1 2) (1 2 3)))
(TEST "(= (1 2 3) (1 2))" Nil (= (1 2 3) (1 2)))
(TEST "(= (1 '2 3) (1 2 3))" Nil (= (1 '2 3) (1 2 3)))
(TEST "(= (1 '2 3) (1 '2 3))" T (= (1 '2 3) (1 '2 3)))
(TEST "(= (1 '2 3 (1 2 3)) (1 '2 3))" Nil (= (1 '2 3 (1 2 3)) (1 '2 3)))
(TEST "(= (1 '2 3 (1 2 3) ) (1 '2 3 (1 2 3)))" T (= (1 '2 3 (1 2 3)) (1 '2 3 (1 2 3))))
(TEST "(cmp)" 0 (cmp))
(TEST "(cmp 1 1)" 0 (cmp 1 1))
(TEST "(cmp 1 2)" -1 (cmp 1 2))
(TEST "(cmp 2 1)" 1 (cmp 2 1))
(TEST "(cmp \"a\" \"a\")" 0 (cmp "a" "a"))
(TEST "(cmp \"b\" \"a\")" 1 (cmp "b" "a"))
(TEST "(cmp \"ab\" \"a\")" 1 (cmp "ab" "a"))
(TEST "(cmp \"ab\" \"b\")" -1 (cmp "ab" "b"))
(TEST "(cmp \"abcdef\" \"b\")" -1 (cmp "abcdef" "b"))
(TEST "(cmp \"abcdefghij\" \"b\")" -1 (cmp "abcdefghij" "b"))
(TEST "(cmp \"aaaaaaaaaaaaaaaa\" (pack (make (do 16 (link \"a\")))))" 0 (cmp "aaaaaaaaaaaaaaaa" (pack (make (do 16 (link "a"))))))
(TEST "(cmp () ())" 0 (cmp () ()))
(TEST "(cmp (1 2 3) (1 2 3))" 0 (cmp (1 2 3) (1 2 3)))
(TEST "(cmp (1 2) (1 2 3))" 1 (cmp (1 2) (1 2 3)))
(TEST "(cmp (1 2 3) (1 2))" 1 (cmp (1 2 3) (1 2)))
(TEST "(cmp (1 '2 3) (1 2 3))" -1 (cmp (1 '2 3) (1 2 3)))
(TEST "(cmp (1 2 3) (1 '2 3))" 1 (cmp (1 2 3) (1 '2 3)))
(TEST "(cmp (1 '2 3) (1 '2 3))" 0 (cmp (1 '2 3) (1 '2 3)))
(TEST "(cmp (1 '2 3 (1 2 3)) (1 '2 3))" 1 (cmp (1 '2 3 (1 2 3)) (1 '2 3)))
(TEST "(cmp (1 '2 3 (1 2 3) ) (1 '2 3 (1 2 3)))" 0 (cmp (1 '2 3 (1 2 3)) (1 '2 3 (1 2 3))))
(TEST "(not T)" Nil (not T))
(TEST "(not Nil)" T (not Nil))
(TEST "(not (= 1 0))" T (not (= 1 0)))
(TEST "(and Nil Nil)" Nil (let (A Nil B Nil) (and A B)))
(TEST "(and Nil T)" Nil (let (A Nil B T) (and A B)))
(TEST "(and T Nil)" Nil (let (A T B Nil) (and A B)))
(TEST "(and T T)" T (let (A T B T) (and A B)))

################################################################################
# for mapcar loop
################################################################################
(TEST "for I 10"  55 (let X 0 (for I 10 (setq X (+ X I)))))
(TEST "for I (1..10)"  55 (let X 0 (for I (1 2 3 4 5 6 7 8 9 10) (setq X (+ X I)))))

(TEST "mapcar" (2 3 4) (mapcar '((X) (+ 1 X)) (1 2 3)))

# loop is infinite loop, so no testing :)

################################################################################
# inc/dec
################################################################################
(TEST "(inc 1)" 2 (inc 1))
(TEST "(let X 1 (inc X))" 2 (let X 1 (inc X)))
(TEST "(dec 1)" 0 (dec 1))
(TEST "(dec 0)" -1 (dec 0))
(TEST "(let X 1 (dec X))" 0 (let X 1 (dec X)))

################################################################################
# eval / call / run
################################################################################

(TEST "run" "HELLO 11" (let Y '((pack "HELLO " (+ 1 X)))  (let X 10 (run Y))))

(TEST "eval" 3 (let E '(+ a b) (let (a 1 b 2) (eval E))))

(TEST "(cmp (call \"exit 1\") 0)" 1 (cmp (call "exit 1") 0))

################################################################################
# if / cond
################################################################################
(de RETURN_TRUE () (link "C") T)
(de RETURN_FALSE () (link "C") Nil)
(TEST "if true path" '("C" "TRUE") (make (if (RETURN_TRUE) (link "TRUE") (link "FALSE"))))
(TEST "if false path" '("C" "FALSE") (make (if (RETURN_FALSE) (link "TRUE") (link "FALSE"))))
(TEST "(cond ((= 3 4) 1) ((= 3 3) 2) (T 3) )" 2 (cond ((= 3 4) 1) ((= 3 3) 2) (T 3) ))
(TEST "(cond ((= 3 4) 1) ((= 3 5) 2) (T 3) )" 3 (cond ((= 3 4) 1) ((= 3 5) 2) (T 3) ))

################################################################################
# pack / chop
################################################################################

(TEST "pack num str" "1" (pack "1"))
(TEST "pack num num" "1" (pack 1))
(TEST "pack num list" "123" (pack '(1 2 3)))
(TEST "pack num list" "123" (pack (make (for I 3 (link (pack I))))))

(TEST "(chop)" Nil (chop))
(TEST "chop" '("A" "B" "C") (chop "ABC"))



(out "TESTFILE" (let X (sb "12abcdef" 16) (wr X)))
(TEST "read 4 bytes" (sb (sb "12abcdef" 16)) (in "TESTFILE" (sb (car (rd 4)) 10 )))
(let BYTES_WRITTEN (out "TESTFILE" (let X (sb "aabbccddeeffaabb" 16) (wr X)))
(TEST (pack "read " BYTES_WRITTEN " bytes") (sb (sb "aabbccddeeffaabb" 16)) (in "TESTFILE" (sb (car (rd BYTES_WRITTEN)) 10 ))))
(out "TESTFILE" (wr 256))
(TEST "rd 100 when there are only 2" 256 (in "TESTFILE" (car (rd 100))))
(out "TESTFILE" (prinl "ABCDEF"))
(TEST "slurp using rd" (pack "ABCDEF" (char 10)) (in "TESTFILE" (pack (make (let Y (rd 1) (while (car (cdr Y)) (link (char (car Y))) (setq Y (rd 1))))))))


################################################################################
# thread / sleep /tid
################################################################################

(out "TESTFILE") # empty out the file
(let (A (thread () (out "TESTFILE" (prinl "HELLO")))
      B (sleep 100)
      C (in "TESTFILE" (pack (line))))
    (TEST "write file in a thread" "HELLO" C))
(TEST "tid" 1 (cmp (tid) 0))

################################################################################
# load
################################################################################

(setq F Nil)
(setq G Nil)
(out "TESTFILE" (prinl "(setq F 10)"))
(load "TESTFILE")
(TEST "load test #1" 10 F)
(out "TESTFILE" (prinl "(de G (A B) (+ A B))"))
(load "TESTFILE")
(TEST "load test #2" 10 (G 7 3))

################################################################################
# net
################################################################################

(setq TEST_PORT 8080)
(de SRV (PORT)
    (let (B (bind PORT)
          S (listen B))
        (socket S
            (let L (line)
                (prinl "ECHO: " L)))
    )
)


(do 1
    (thread () (SRV TEST_PORT))
    (sleep 100)

    (let (X Nil
          S (connect "127.0.0.1" TEST_PORT))
        (socket S
            (prinl "HELLO")
            (setq X (pack (line))))
        (TEST "networking" "ECHO: HELLO" X))
)

(de SRV_BIN (PORT)
    (let (B (bind PORT)
          S (listen B))
        (socket S (for I 256 (let X (car (rd 1))  (wr X))))))

(do 1
    (thread () (SRV_BIN 9090))
    (sleep 100)

    (let (X 0
          S (connect "127.0.0.1" 9090))
        (socket S (for I 256 (wr (- I 1)) (setq X  (+ X (car (rd 1)))) (out "+CLIENT" (prinl X)) ))
        (TEST "networking binary" (let Y 0 (for I 255 (setq Y (+ I Y)))) X)))


################################################################################
# end of test
################################################################################

(hs)
(prinl "PASS_COUNT = " PASS_COUNT ", FAIL_COUNT = " FAIL_COUNT);
(bye)
