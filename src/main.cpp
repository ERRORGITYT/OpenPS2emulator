#include <iostream>
#include <chrono>
#include <thread>
#include "core/memory.h"
#include "core/cpu.h"
#include "display.h"

using namespace core;

int main(int argc, char* argv[]) {
    // 1. Setup Hardware
    Memory memory;
    CPU cpu(memory);
    Display display;

    // 2. Initialize Display (SDL)
    if (!display.init()) {
        std::cerr << "Failed to initialize display!" << std::endl;
        return -1;
    }

    // 3. Load the ROM
    // For now, we'll look for a file named "test.ch8"
    // You can change this to any CHIP-8 ROM you have.
    if (!memory.loadROM("test.ch8")) {
        std::cout << "No ROM found, or failed to load. Running empty CPU cycle..." << std::endl;
    } else {
        std::cout << "ROM loaded successfully!" << std::endl;
    }

    bool quit = false;
    SDL_Event e;

    // 4. The Main Emulation Loop
    while (!quit) {
        // Handle SDL Events (so the window doesn't freeze)
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Run one CPU cycle (Fetch, Decode, Execute)
        cpu.cycle();

        // Update timers at 60Hz
        cpu.updateTimers();

        // Refresh the screen
        display.clear();
        // (Later, we'll add the logic to draw the CPU's display buffer here)
        display.present();

        // Simple delay to prevent the CPU from running too fast
        // CHIP-8 usually runs around 500-700Hz
        std::this_thread::sleep_for(std::chrono::microseconds(1200));
    }

    std::cout << "Shutting down emulator..." << std::endl;
    return 0;
}
