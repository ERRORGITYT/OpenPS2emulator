#include "core/memory.h"
#include "core/cpu.h"
#include "display.h"
#include "input.h"
#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include <iomanip>

/**
 * OpenPS2emulator - CHIP-8 Core Execution Entry
 * High-performance cycle-accurate timing implementation.
 */

int main(int argc, char* argv[]) {
    std::cout << "--- OpenPS2emulator CHIP-8 Core Starting ---" << std::endl;

    // 1. ARGUMENT PARSING
    // Allows you to run: ./OpenPS2emulator roms/pong.ch8
    std::string romPath = "test.ch8"; 
    if (argc > 1) {
        romPath = argv[1];
    }

    // 2. HARDWARE INITIALIZATION
    core::Memory memory;
    core::CPU cpu(memory);
    Display display;
    core::Input input;

    if (!display.init()) {
        std::cerr << "[FATAL] SDL2 Video Driver Initialization Failed!" << std::endl;
        return -1;
    }

    if (!memory.loadROM(romPath)) {
        std::cerr << "[FATAL] Failed to inject binary: " << romPath << std::endl;
        return -1;
    }

    // 3. EMULATION STATE
    bool quit = false;
    uint32_t totalCycles = 0;
    
    // Timing control (60Hz for timers, ~500Hz-700Hz for CPU)
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    auto lastTimerTime = std::chrono::high_resolution_clock::now();

    std::cout << "[SYSTEM] Entering Main Execution Loop..." << std::endl;

    // 4. THE MASTER LOOP
    while (!quit) {
        auto currentTime = std::chrono::high_resolution_clock::now();

        // --- CPU CYCLE HANDLING (~500Hz) ---
        float cycleDelta = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
        if (cycleDelta >= 2.0f) { // 2ms = 500Hz
            
            // Poll hardware input
            input.update(quit, cpu.getKeys());

            if (!quit) {
                // Batch execution for performance
                for (int i = 0; i < 8; i++) {
                    cpu.cycle();
                    totalCycles++;
                }
            }

            // Sync graphics
            display.render(cpu.getDisplayBuffer(), 64, 32);
to:
            display.present();
            
            lastCycleTime = currentTime;
        }

        // --- TIMER UPDATES (Fixed 60Hz) ---
        float timerDelta = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastTimerTime).count();
        if (timerDelta >= 16.67f) { // 16.67ms = 60Hz
            cpu.updateTimers();
            lastTimerTime = currentTime;
        }

        // 5. PERFORMANCE TELEMETRY (Print stats every 10,000 cycles)
        if (totalCycles % 10000 == 0 && totalCycles > 0) {
            std::cout << "[LOG] Uptime: " << std::fixed << std::setprecision(2) 
                      << totalCycles / 500.0f << "s | Cycles: " << totalCycles << "\r" << std::flush;
        }

        // Prevent 100% CPU usage on the Helio G85
        std::this_thread::yield();
    }

    std::cout << "\n[SYSTEM] Execution Terminated. Total Cycles: " << totalCycles << std::endl;
    return 0;
}
