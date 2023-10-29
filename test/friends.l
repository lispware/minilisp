(be likes (John Susie))
(be likes (John Pizza))
(be likes (Susie John))

(be friends (@X @Y)
   (likes @X @Y)
   (likes @Y @X) )

(be hates (@X @Y)
   (not (likes @X @Y)) )

(be enemies (@X @Y)
   (not (likes @X @Y))
   (not (likes @Y @X)) )


(let TF "TESTFILE"
	(out TF (? (likes John @X)))
	(prinl (if (= " @X=Susie @X=Pizza" (pack (in TF (line)))) "PASS" "FAIL")))

(be is_digesting (@X @Y)
   (just_ate @X @Y) )

(be is_digesting (@X @Y)
   (just_ate @X @Z)
   (is_digesting @Z @Y) )

(be just_ate (Mosquito (blood John)))
(be just_ate (Frog Mosquito ))
(be just_ate (Stork Frog))

(let TF "TESTFILE"
	(out TF (? (is_digesting Stork @X)))
	(prinl (if (= " @X=Frog @X=Mosquito @X=(blood John)" (pack (in TF (line)))) "PASS" "FAIL")))


(be tran (eins one))
(be tran (zwei two))
(be tran (drei three))
(be tran (vier four))
(be tran (fuenf five))
(be tran (sechs six))
(be tran (sieben seven))
(be tran (acht eight))
(be tran (neun nine))
(be tran (zehn ten))

(be listtran (NIL NIL))

(be listtran ((@Hg . @Tg) (@He . @Te))
   (tran @Hg @He))

(be listtran (NIL NIL))

(be listtran ((@Hg . @Tg) . (@He  . @Te))
   (tran @Hg @He)
   (listtran @Tg @Te) )

 (? (listtran (eins zwei drei) @X))

(prinl)
