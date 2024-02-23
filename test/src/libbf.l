(let X (BN "123.456") 
    (finally
        (BNdelete X)
        (test "123.456" (BNtoa X 6))))

(let X (BN "ff" 16)
    (finally (BNdelete X)
    (test "255" (BNtoa X 3))))

(let (N1 (BN 1)
      N2 (BN 2)
      N3 (BN+ N1 N2))
   (finally
    (BNdelete N1 N2 N3)
    (test "3" (BNtoa N3 1))))

(let (N1 (BN 1)
      N2 (BN 2)
      N3 (BN 3)
      N4 (BN+ N1 N2 N3))
   (finally
    (BNdelete N1 N2 N3 N4)
    (test "6" (BNtoa N4 1))))
