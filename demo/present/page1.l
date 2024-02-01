(SDL_SetRenderDrawColor RENDERER 50 50 200 255)
(SDL_RenderFillRect RENDERER 100 100 200 200)

(g-clear)
(draw-str "hello" 100 200 FONT_WIDTH FONT_HEIGHT)

(SDL_RenderPresent RENDERER)
