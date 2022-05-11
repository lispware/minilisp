(load "SDL.l")

(SDL_Init (| SDL_INIT_VIDEO SDL_INIT_AUDIO))

(setq HEIGHT 1000)
(setq WIDTH 1500)
(setq FONT_WIDTH 10)
(setq FONT_HEIGHT 18)
(setq MAGNIFICATION 1)
(setq EDITOR_Y (- HEIGHT (* MAGNIFICATION FONT_HEIGHT)))
(setq OUTPUT_Y (- HEIGHT (* 3 MAGNIFICATION FONT_HEIGHT)))

(setq WIN (SDL_CreateWindow "HELLO" WIDTH HEIGHT 0 0 0))
(setq RENDERER (SDL_CreateRenderer WIN -1 0))
(setq MUTEX (SDL_CreateMutex))



(setq IMAGE_SURFACE (SDL_LoadBMP "kong.bmp"))
(setq TEXTURE (SDL_CreateTextureFromSurface RENDERER IMAGE_SURFACE))
(SDL_FreeSurface IMAGE_SURFACE)

(setq SPACES (pack (make (do (car (/ WIDTH FONT_WIDTH)) (link " ")))))

(de reverse (L)
    (let R Nil
        (for I L
            (setq R (cons I R)))
    R))

(de putPixel (X Y R G B)
    (SDL_RenderDrawLine RENDERER  X Y X Y R G B))

(de line (X1 Y1 X2 Y2 R G B)
    (let CF '((C) (if C C 255))
        (SDL_SetRenderDrawColor RENDERER (CF R) (CF G) (CF B))
        (SDL_RenderDrawLine RENDERER X1 Y1 X2 Y2)))

(de point (X1 Y1 R G B)
    (let CF '((C) (if C C 255))
        (SDL_SetRenderDrawColor RENDERER (CF R) (CF G) (CF B))
        (SDL_RenderDrawPoint RENDERER X1 Y1)))

(de box (X Y S R1 G1 B1)
    (let (CF '((C) (if C C 255))
          R (CF R1)
          G (CF G1)
          B (CF B1))
        (SDL_RenderDrawLine RENDERER X Y (+ X S) Y  R G B)
        (SDL_RenderDrawLine RENDERER X Y X (+ Y S)  R G B)
        (SDL_RenderDrawLine RENDERER X (+ Y S) (+ X S) (+ Y S)  R G B)
        (SDL_RenderDrawLine RENDERER (+ X S) (+ Y S) (+ X S) Y  R G B)))

(de clear (R G B)
    (let CF '((C) (if C C 255))
        (SDL_SetRenderDrawColor RENDERER (CF R) (CF G) (CF B)))

    (SDL_RendererClear RENDERER)
    (SDL_RendererPresent RENDERER))

(de update () (SDL_RendererPresent RENDERER))

(de dd ()
    (SDL_RenderCopy RENDERER TEXTURE Nil (100 100 400 400)))
    #(SDL_RenderCopy RENDERER TEXTURE (100 100 200 200) (100 100 100 100)))


(de audioSetup ()
    (let (
        X (LoadWAV "C4.wav")
        BUF (car X)
        X (cdr X)
        BUFLEN (car X)
        X (cdr X)
        FREQ (car X)
        X (cdr X)
        FORMAT (car X)
        X (cdr X)
        CHANNELS (car X)
        X (cdr X)
        SAMPLES (car X)
        )
    (setq BUFFER BUF)
    (setq BUFFER_LENGTH BUFLEN)
    (setq AD (SDL_OpenAudioDevice FREQ FORMAT CHANNELS SAMPLES))))

(de audioDemo (N)
    #(do 1 (for I N (do 1 (SDL_LockMutex MUTEX) (PlayWAV AD) (SDL_UnlockMutex)))) (sleep 3000) )
    (thread () (do 1 (for I N (do 1 (PlayWAV AD))))  ))
(de audioStop ()
    (SDL_ClearQueuedAudio AD))

(audioSetup)

(de demo ()
    (SDL_LockMutex MUTEX)
    (clear 0 0 255)
    (SDL_UnlockMutex MUTEX)
    (for I (list (255 0 0) (0 255 0) (255 255 255))
        (SDL_LockMutex MUTEX)
        (SDL_SetRenderDrawColor RENDERER (car I) (car (cdr I)) (car (cdr (cdr I))))
        (box 100 100 200)
        (SDL_RendererPresent RENDERER)
        (SDL_UnlockMutex MUTEX)
        (SDL_Delay 400)))

(de write (X Y S)
    (WriteString RENDERER X Y 255 255 255 0 0 255 MAGNIFICATION S))

(de updateLine (X)
    (PlayWAV AD)
    (write 0 EDITOR_Y SPACES)
    (if X (write 0 EDITOR_Y (pack X)))
    (SDL_RendererPresent RENDERER))

(de updateOutput (X)
    (write 0 OUTPUT_Y SPACES)
    (if X (write 0 OUTPUT_Y (pack X)))
    (SDL_RendererPresent RENDERER))

(write 0 EDITOR_Y SPACES)

(de quitApp (W)
    (SDL_DestroyWindow W)
    (SDL_Quit))

(de processWindowEvent (EVENT WIN)
    (if (= EVENT SDL_WINDOWEVENT_CLOSE) (quitApp WIN)))

(de processTextInput (EVENT WIN)
    (SDL_LockMutex MUTEX)
    (updateLine (reverse (setq R (cons (char EVENT) R))))
    (SDL_UnlockMutex MUTEX))

(de processKeyDownEvent (EVENT WIN)
    (SDL_LockMutex MUTEX)
    (if (= EVENT SDLK_BACKSPACE)
        (do 1 (setq R (cdr R)) (updateLine (reverse R))))
    (if (= EVENT SDLK_RETURN)
        (do 1 (if R (updateOutput (pack (load (pack (reverse R)))))) (setq R Nil) (SDL_RendererPresent RENDERER)))
    (SDL_UnlockMutex MUTEX))


(de EVENT_NAME (E)
    (let R (if (= E WINDOW_EVENT) "WINDOW_EVENT" Nil)
        (if (= E SDL_KEYDOWN) (setq R "KEY_DOWN"))
        R))


(loop

    (while (SDL_PollEvent)
        (let (EVENT_TYPE (car @)
              EVENT  (car (cdr @)))

            (prinl "TYPE = " (EVENT_NAME EVENT_TYPE) ", EVENT = " EVENT)

            (if (= EVENT_TYPE SDL_WINDOWEVENT)
                (processWindowEvent EVENT WIN))

            (if (= EVENT_TYPE SDL_TEXTINPUT)
                (processTextInput EVENT WIN))

            (if (= EVENT_TYPE SDL_KEYDOWN)
                (processKeyDownEvent EVENT WIN)))))
