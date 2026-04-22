#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include <array>
#include <map>
#include <cstdint>

namespace core {

class Input {
public:
    Input();
    void update(bool& quit, std::array<uint8_t, 16>& keys);

private:
    // Maps modern keys to CHIP-8 hex keys
    std::map<SDL_Keycode, uint8_t> keymap;
};

} // namespace core

#endif
