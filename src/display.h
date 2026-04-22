#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>

namespace core {

class Display {
public:
    Display();
    ~Display();

    bool init();
    void clear();
    void present();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
};

} // namespace core

#endif
