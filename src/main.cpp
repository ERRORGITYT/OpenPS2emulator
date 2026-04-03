#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <stdio.h>

// PS2 Constants
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 448

int main(int argc, char* argv[]) {
    // Basic Init
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL Init Failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // OpenGL Attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Create Window
    SDL_Window* window = SDL_CreateWindow(
        "OpenPS2emulator",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    // Inside your main function in main.cpp
SDL_Window* window = initialize_sdl(); // The function from your screenshot

bool running = true;
SDL_Event event;

while (running) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) running = false;
    }
    
    // Rendering calls go here
}

    if (!window) {
        std::cerr << "Window creation failed!" << std::endl;
        SDL_Quit();
        return 1;
    }

    // The Graphics Context
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        std::cerr << "GL Context creation failed!" << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Main Loop
    bool isRunning = true;
    SDL_Event event;

    while (isRunning) {
        // Handle Events (Prevents "Not Responding" freeze)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
        }

        // Clear the screen to a visible color (Not black!)
        // This confirms the GPU is actually rendering.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap buffers to display the frame
        SDL_GL_SwapWindow(window);

        // Limit to ~60FPS
        SDL_Delay(16);
    }

    // Clean Exit
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
