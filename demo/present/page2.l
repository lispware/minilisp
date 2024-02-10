(SDL_SetRenderDrawColor RENDERER 0 0 0 255)
(SDL_RenderFillRect RENDERER 100 100 200 200)

(g-clear)
(draw-str "hello" 100 200 (* FONT_WIDTH 1) (* FONT_HEIGHT 1))

(SDL_RenderPresent RENDERER)
