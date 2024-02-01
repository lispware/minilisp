(SDL_SetRenderDrawColor RENDERER 50 50 200 255)
(SDL_RenderFillRect RENDERER 100 100 200 200)

(g-clear)
(g-draw-str "hello" 100 200)

(SDL_RenderPresent RENDERER)
