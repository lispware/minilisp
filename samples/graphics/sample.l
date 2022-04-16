(load "SDL.l")

(SDL_Init -1)

(setq HEIGHT 1000)
(setq WIDTH 1500)
(setq FONT_WIDTH 10)
(setq FONT_HEIGHT 18)
(setq MAGNIFICATION 1)
(setq EDITOR_Y (- HEIGHT (* MAGNIFICATION FONT_HEIGHT)))
(setq OUTPUT_Y (- HEIGHT (* 3 MAGNIFICATION FONT_HEIGHT)))

(setq WIN (SDL_CreateWindow "HELLO" WIDTH HEIGHT 0 0 0))
(setq RENDERER (SDL_CreateRenderer WIN -1 0))

(SDL_GetSurface WIN)

(setq SPACES (pack (make (do (car (/ WIDTH FONT_WIDTH)) (link " ")))))

(de reverse (L)
    (let R Nil
        (for I L
            (setq R (cons I R)))
    R))

(de putPixel (X Y R G B)
    (sdlDrawLine  X Y X Y R G B))

(de line (X1 Y1 X2 Y2 R G B)
    (let CF '((C) (if C C 255))
        (sdlDrawLine X1 Y1 X2 Y2 (CF R) (CF G) (CF B))))

(de box (X Y S R1 G1 B1)
    (let (CF '((C) (if C C 255))
          R (CF R1)
          G (CF G1)
          B (CF B1))
        (sdlDrawLine X Y (+ X S) Y  R G B)
        (sdlDrawLine X Y X (+ Y S)  R G B)
        (sdlDrawLine X (+ Y S) (+ X S) (+ Y S)  R G B)
        (sdlDrawLine (+ X S) (+ Y S) (+ X S) Y  R G B)))

(de clear (R G B)
    (let CF '((C) (if C C 255))
     (sdlClearWindow (CF R) (CF G) (CF B)))
    (SDL_RendererPresent RENDERER))

(de update () (SDL_RendererPresent RENDERER))

(de demo ()
    (clear 0 0 255)
    (for I (list (255 0 0) (0 255 0) (255 255 255))
        (box 100 100 200 (car I) (car (cdr I)) (car (cdr (cdr I))))
        (SDL_RendererPresent RENDERER)
        (sdlDelay 400)))

(de write (X Y S)
    (sdlWriteString X Y 255 255 255 0 0 255 MAGNIFICATION S))

(de updateLine (X)
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
    (updateLine (reverse (setq R (cons (char EVENT) R)))))

(de processKeyDownEvent (EVENT WIN)
    (if (= EVENT SDLK_BACKSPACE)
        (do 1 (setq R (cdr R)) (updateLine (reverse R))))
    (if (= EVENT SDLK_RETURN)
        (do 1 (if R (updateOutput (pack (load (pack (reverse R)))))) (setq R Nil) (SDL_RendererPresent RENDERER))))

(loop
    (while (SDL_PollEvent)
        (let (EVENT_TYPE (car @)
              EVENT  (car (cdr @)))

            (prinl EVENT_TYPE ", "  EVENT)

            (if (= EVENT_TYPE SDL_WINDOWEVENT)
                (processWindowEvent EVENT WIN))

            (if (= EVENT_TYPE SDL_TEXTINPUT)
                (processTextInput EVENT WIN))

            (if (= EVENT_TYPE SDL_KEYDOWN)
                (processKeyDownEvent EVENT WIN)))))
