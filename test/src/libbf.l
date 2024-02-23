(let X (BN "123.456") 
    (test "123.456" (BNtoa X 6))
    (finally (BNdelete X)))

(let X (BN "ff" 16)
    (test "255" (BNtoa X 3))
    (finally (BNdelete X)))
