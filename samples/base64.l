# ref - https://rosettacode.org/wiki/Base64_decode_data#PicoLisp

(setq Base64Table `'(chop "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" ))

(de SkipNonBase64Chars (S)
    (make (for I (chop S)
        (let (J (index I Base64Table))
            (if J (link (dec J)))))))

(de decode64 (S)
   (let S (SkipNonBase64Chars S)
         (make
            (while S
               (let
                  (  A (++ S)
                     B (++ S)
                     C (++ S)
                     D (++ S)
                     )
                  (link
                     (| (>> -2 A) (>> 4 B)))
                  (and
                     C
                     (link
                           (| (>> -4 (& B 15)) (>> 2 C)) ) 
                     D
                     (link
                        (| (>> -6 (& C 3)) D)) ) ) ) ) ) )
         
(for I (decode64 "SGVsbG8gYmFzZTY0IHdvcmxkIQo=")  (wr I))

(bye)
