#ifndef CPU_H
#define CPU_H

#include "memory.h"
#include <cstdint>
#include <array>
#include <random>

namespace core {

class CPU {
public:
    explicit CPU(Memory& mem);
    ~CPU() = default;

    void reset();
    void cycle();
    void updateTimers();

    // Accessors for Main Loop & Display
    const uint8_t* getDisplayBuffer() const { return display.data(); }
    std::array<uint8_t, 16>& getKeys() { return keys; }
    bool isSoundActive() const { return soundTimer > 0; }

private:
    Memory& memory;

    // Registers & Pointers
    uint8_t V[16];          // General purpose registers V0-VF
    uint16_t I;             // Index register
    uint16_t PC;            // Program counter
    uint8_t delayTimer;
    uint8_t soundTimer;

    // Hardware Stack
    uint16_t stack[16];
    uint8_t sp;             // Stack pointer

    // IO Buffers
    std::array<uint8_t, 64 * 32> display;
    std::array<uint8_t, 16> keys;

    // RNG Engine
    std::mt19937 rng;
    std::uniform_int_distribution<uint8_t> dist;

    // Instruction Decoder
    void executeOpcode(uint16_t opcode);
};

} // namespace core

#endif
