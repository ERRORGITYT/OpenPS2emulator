#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <vector>
#include "memory.h" // We need access to the memory we just made!

namespace core {

class CPU {
private:
    // --- CHIP-8 Registers ---
    
    // 16 General Purpose 8-bit registers (V0 to VF)
    uint8_t V[16];

    // Index register (16-bit, but only 12 bits are usually used for addresses)
    uint16_t I;

    // Program Counter (points to the current instruction in memory)
    uint16_t PC;

    // --- The Stack ---
    
    // The stack is used to remember where to return after a subroutine (function call)
    uint16_t stack[16];
    uint8_t  stackPointer;

    // --- Timers ---
    
    // Both count down at 60Hz and stop at 0
    uint8_t delayTimer;
    uint8_t soundTimer;

    // A reference to our Memory object so the CPU can read/write
    Memory& memory;

public:
    // The CPU needs a reference to Memory to function
    CPU(Memory& mem);
    ~CPU() = default;

    // Resets registers and sets PC to 0x200 (where the game starts)
    void reset();

    // The core cycle: Fetch -> Decode -> Execute
    void cycle();

    // Updates the 60Hz timers (called from main loop)
    void updateTimers();
};

} // namespace core

#endif // CPU_H
