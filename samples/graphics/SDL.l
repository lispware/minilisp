# include/SDL_events.h
(setq SDL_INIT_AUDIO      16)
(setq SDL_INIT_VIDEO      32)
(setq SDL_WINDOWEVENT     (sb "200" 16))
(setq SDL_KEYDOWN         (sb "300" 16))
(setq SDL_KEYUP           (sb "301" 16))
(setq SDL_TEXTEDITING     (sb "302" 16))
(setq SDL_TEXTINPUT       (sb "303" 16))
(setq SDL_KEYMAPCHANGED   (sb "304" 16))

# include/SDL_video.h
(setq SDL_WINDOWEVENT_NONE             0)
(setq SDL_WINDOWEVENT_SHOWN            1)
(setq SDL_WINDOWEVENT_HIDDEN           2)
(setq SDL_WINDOWEVENT_EXPOSED          3)
(setq SDL_WINDOWEVENT_MOVED            4)
(setq SDL_WINDOWEVENT_RESIZED          5)
(setq SDL_WINDOWEVENT_SIZE_CHANGED     6)
(setq SDL_WINDOWEVENT_MINIMIZED        7)
(setq SDL_WINDOWEVENT_MAXIMIZED        8)
(setq SDL_WINDOWEVENT_RESTORED         9)
(setq SDL_WINDOWEVENT_ENTER           10)
(setq SDL_WINDOWEVENT_LEAVE           11)
(setq SDL_WINDOWEVENT_FOCUS_GAINED    12)
(setq SDL_WINDOWEVENT_FOCUS_LOST      13)
(setq SDL_WINDOWEVENT_CLOSE           14)
(setq SDL_WINDOWEVENT_TAKE_FOCUS      15)
(setq SDL_WINDOWEVENT_HIT_TEST        16)
(setq SDL_WINDOWEVENT_ICCPROF_CHANGED 17)
(setq SDL_WINDOWEVENT_DISPLAY_CHANGED 18)

# include/SDL_keycode.h
(setq SDLK_RETURN     13) # '\r'
(setq SDLK_ESCAPE     (sb "1b" 16)) #'\x1B'
(setq SDLK_BACKSPACE  8) # '\b'
(setq SDLK_TAB        9) # '\t'
(setq SDLK_SPACE     32)

