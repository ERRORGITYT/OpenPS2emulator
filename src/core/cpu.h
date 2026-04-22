#ifndef CPU_H
#define CPU_H

// --- ENGINE INCLUDES ---
#include "memory.h"
#include <random>       // For Opcode Cxkk (RND Vx, byte)
#include <chrono>       // High-precision timing for 60Hz timers
#include <functional>   // Function pointers and lambda handling
#include <stack>        // Alternative stack implementation
#include <map>          // For mapping opcodes to debug strings
#include <thread>       // For cycle-speed management
#include <atomic>       // Thread-safe flags for the CPU state

namespace core {

class CPU {
public:
    explicit CPU(Memory& mem);
    ~CPU() = default;

    void reset();
    void cycle();
    void updateTimers();

private:
    Memory& memory;

    // Registers
    uint8_t V[16];      // V0 through VF
    uint16_t I;         // Index register
    uint16_t PC;        // Program Counter
    uint8_t delayTimer;
    uint8_t soundTimer;

    // Hardware Stack
    uint16_t stack[16];
    uint8_t stackPointer;

    // Random Number Engine
    std::mt19937 rng;
    std::uniform_int_distribution<uint8_t> dist;

    // Internal execution methods
    void executeOpcode(uint16_t opcode);
};

} // namespace core

#endif
