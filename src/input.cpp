#include "input.h"

namespace core {

Input::Input() {
    // Classic layout mapping:
    // 1 2 3 C  ->  1 2 3 4
    // 4 5 6 D  ->  Q W E R
    // 7 8 9 E  ->  A S D F
    // A 0 B F  ->  Z X C V
    keymap = {
        {SDLK_1, 0x1}, {SDLK_2, 0x2}, {SDLK_3, 0x3}, {SDLK_4, 0xC},
        {SDLK_q, 0x4}, {SDLK_w, 0x5}, {SDLK_e, 0x6}, {SDLK_r, 0xD},
        {SDLK_a, 0x7}, {SDLK_s, 0x8}, {SDLK_d, 0x9}, {SDLK_f, 0xE},
        {SDLK_z, 0xA}, {SDLK_x, 0x0}, {SDLK_c, 0xB}, {SDLK_v, 0xF}
    };
}

void Input::update(bool& quit, std::array<uint8_t, 16>& keys) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) quit = true;

        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            auto it = keymap.find(event.key.keysym.sym);
            if (it != keymap.end()) {
                keys[it->second] = (event.type == SDL_KEYDOWN) ? 1 : 0;
            }
            
            // ESC to quit
            if (event.key.keysym.sym == SDLK_ESCAPE) quit = true;
        }
    }
}

} // namespace core
