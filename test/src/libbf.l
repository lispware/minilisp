(let X (BN "123.456") 
    (test "123.456" (BNtoa X 6))
    (finally (BNdelete X)))

(let X (BN "ff" 16)
    (test "255" (BNtoa X 3))
    (finally (BNdelete X)))

(let (N1 (BN 1)
      N2 (BN 2)
      N3 (BN+ N1 N2))
   (test "3" (BNtoa N3 1))
   (finally
    (BNdelete N1)
    (BNdelete N2)
    (BNdelete N3)))

(let (N1 (BN 1)
      N2 (BN 2)
      N3 (BN 3)
      N4 (BN+ N1 N2 N3))
   (test "6" (BNtoa N4 1))
   (finally
    (BNdelete N1)
    (BNdelete N2)
    (BNdelete N3)
    (BNdelete N4)))
