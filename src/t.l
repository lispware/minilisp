# The test file

(setq L1 (load 1.l))
(prinl "load returned " L1)
(setq L2 (load 2.l))
(prinl "load returned " L2)

(de F (X Y) 
    (let A 10 
         (let (AA 20 BB 30)
        (+ A AA BB X Y))))

(setq G (quote (X)
 (+ 1 X)))

(prinl (G 1))

(setq H '( (X) (+ 1 X)))
(prinl (H 1))


(prinl "TESTING VARIABLE ARGS")

(de V A
    (for i A
        (prinl "ELEMENT " i)
     )
)
(V 1 2 3 A B C)

(prinl (F 1 2))

(prinl (= 1 1))

(if (= 1 1) (if (= 1 2) (prinl 11) (prinl 12)) (prinl 13))
(if (= 1 2) (if (= 1 2) (prinl 11) (prinl 12)) (prinl 13))
(if (= 1 1) (if (= 1 1) (prinl 11) (prinl 12)) (prinl 13))

(do 2
(prinl (F 1 2)))


(de M X (prinl X))
(M 1)
(M 1 2 3)


(prinl 123)
(setq L1 (cons 1 (cons 2 (cons 3 Nil))))
(setq L2 (cdr L1))
(setq L3 (cdr L2))
(prinl (car L1))
(prinl (car L2))
(prinl (car L3))
(prinl 456)


(let X 0 (do 3 (prinl X) (setq X (+ 1 X))))

(de DO (N F)
       (if (= N 0)
           Nil
           (F)
           (DO (- N 1) F)))


(de MKLST (N)
    (make (do N (link 1))))

(de LEN (L)
    (let LL 0 
         (while (not (= Nil L))
             (setq LL (+ 1 LL))
             (setq L (cdr L))
         )
         LL
     )
)

(setq AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA 100)
(setq BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB 123)
(prinl (+ AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB))

(prinl "12345678")
(prinl "1234567812345678")

(prinl (pack 1 2 3 4))
(prinl (pack 1 2 3 4 5))
(prinl (pack 1 2 3 4 5 6 7 8))
(prinl (pack 1 2 3 4 5 6 7 8))
(prinl (pack "1" "2" "3" "4"))
(prinl (pack "1" "2" "3" "4" "5"))
(prinl (pack "1" "2" "3" "4" "5" "6" "7" "8"))
(prinl (pack "1" "2" "3" "4" "5" "6" "7" "8"))
(if (= "hello world" (pack "hello" " " "world"))
    (prinl "pack passed")
    (prinl "pack failed")
    (bye))

(setq longnumber 1234567890)
(prinl (+ (* longnumber 10) 1))

(setq shorttxt "ABCD")
(prinl shorttxt)
(setq longtxt "A REALLY LONG TXT")
(prinl longtxt)

(setq L4 (make (link 1) (link 2) (link 3) ) )
(prinl (car L4))
(prinl (car (cdr L4)))
(prinl (car (cdr (cdr L4))))


(setq  FAC1 (quote (N) (if (= N 0) 1 (* N (FAC1 (- N 1))))))

(prinl (FAC1 5))

(setq  FAC2 (quote (N) (if (= N 0) 1 (* N (FAC2 (- N 1))))))

(prinl (FAC2 5))

(setq MILL (* 1000 1000))

(setq Z (MKLST (+ 1234 MILL)))

(prinl (LEN Z))

(prinl ABCD)

(let L '( 1 2 3) (while L (prinl (car L)) (setq L (cdr L))))

(in "t.l" (do 10 (prinl (char))))
(out "ABCD" (prinl "HELLO WORLD"))
(in "ABCD" (do 11 (prinl (char))))

(out "ABCD"
     (prinl "LINE 1")
     (prinl "A VERY LONG LONG LINE 2"))

(in "ABCD"
    (let (L1 (pack (line))
          L2 (pack (line)))
          (if (= L1 "LINE 1") 
              (if (= L2 "A VERY LONG LONG LINE 2") (prinl "LINE TEST PASSED") (prinl "LINE TEST FAILED IN THE SECOND STRING"))
              (prinl "LINE TEST FAILED IN THE FIRST STRING"))
     ))
         



#### TEST EVAL #

(setq E '(+ a b))
(prinl "EVAL OUTPUT")
(prinl (let (a 1 b 2) (eval E)))


################

 
    


(prinl ABCDEFGHIJ)
(ABCDEFGHIJK)
(ABCDEFGHIJABCDEFGHIJK)

(let X 0
    (for N 10000 (setq X (+ X N))) (prinl X))
(let X 0
    (for N 10000 (setq X (+ X N)) (setq X (+ X N))) (prinl X))
(let X 0
    (for N '( 1 2 3 4 5 6 7 8 9 10) (setq X (+ X N))) (prinl X))

 (let Y '( (prinl "HELLO " (+ 1 X)))  (let X 10 (run Y)))

(hs)
(bye)
