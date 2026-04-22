#include "cpu.h"
#include <iostream>
#include <iomanip>

namespace core {

// Constants and Macros
constexpr uint16_t MASK_NNN = 0x0FFF;
constexpr uint16_t MASK_X   = 0x0F00;
constexpr uint16_t MASK_Y   = 0x00F0;
constexpr uint16_t MASK_N   = 0x000F;
constexpr uint16_t MASK_KK  = 0x00FF;
constexpr size_t MEMORY_SIZE = 4096;
constexpr size_t STACK_DEPTH = 16;

CPU::CPU(Memory& mem) : memory(mem) {
    reset();
}

void CPU::reset() {
    // Standard CHIP-8 start address
    PC = 0x200;
    I = 0;
    stackPointer = 0;
    delayTimer = 0;
    soundTimer = 0;
    
    // Clear registers and stack
    for(int i = 0; i < 16; ++i) {
        V[i] = 0;
        stack[i] = 0;
    }
    std::cout << "CPU Reset Successful. PC set to 0x200" << std::endl;
}

void CPU::cycle() {
    // 1. Fetch
    // We use the memory object to read the 16-bit opcode
    uint16_t opcode = memory.readOpcode(PC);
    
    // Log the opcode for debugging (optional)
    // std::cout << "Executing Opcode: 0x" << std::hex << opcode << std::dec << std::endl;

    // 2. Decode & Execute
    // We move the PC first so it points to the next instruction
    PC += 2;

    uint16_t nnn = opcode & MASK_NNN;
    uint8_t  x   = (opcode & MASK_X) >> 8;
    uint8_t  y   = (opcode & MASK_Y) >> 4;
    uint8_t  kk  = opcode & MASK_KK;
    uint8_t  n   = opcode & MASK_N;

    // The "Big Switch" based on the first hex digit
    switch (opcode & 0xF000) {
        case 0x0000:
            if (opcode == 0x00E0) {
                // CLS: Clear screen (link to display.cpp later)
            } else if (opcode == 0x00EE) {
                // RET: Return from subroutine
                if (stackPointer > 0) {
                    stackPointer--;
                    PC = stack[stackPointer];
                }
            }
            break;

        case 0x1000: // JP addr
            PC = nnn;
            break;

        case 0x2000: // CALL addr
            if (stackPointer < STACK_DEPTH) {
                stack[stackPointer] = PC;
                stackPointer++;
                PC = nnn;
            }
            break;

        case 0x6000: // LD Vx, byte
            V[x] = kk;
            break;

        case 0x7000: // ADD Vx, byte
            V[x] += kk;
            break;

        default:
            // std::cerr << "Unknown opcode: 0x" << std::hex << opcode << std::endl;
            break;
    }
}

void CPU::updateTimers() {
    if (delayTimer > 0) delayTimer--;
    if (soundTimer > 0) soundTimer--;
}

} // namespace core
