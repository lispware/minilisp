(loop 
    (sdlpoll)
    (if (sdlisclose) (bye) (prinl "NOT CLOSE")))
