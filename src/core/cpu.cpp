#include "cpu.h"
#include <iostream>
#include <cstring> // For memset

namespace core {

// These constants help decode the 16-bit opcodes
// Example: Opcode 0xA2F0 -> nnn = 0x2F0, x = 2, kk = 0xF0
constexpr uint16_t MASK_NNN = 0x0FFF; // Address
constexpr uint16_t MASK_X   = 0x0F00; // Register Vx
constexpr uint16_t MASK_Y   = 0x00F0; // Register Vy
constexpr uint16_t MASK_N   = 0x000F; // 4-bit constant
constexpr uint16_t MASK_KK  = 0x00FF; // 8-bit constant

CPU::CPU(Memory& mem) : memory(mem) {
    reset();
}

void CPU::reset() {
    PC = 0x200; // Games start at 0x200
    I = 0;
    stackPointer = 0;
    delayTimer = 0;
    soundTimer = 0;

    // Clear registers and stack
    std::memset(V, 0, sizeof(V));
    std::memset(stack, 0, sizeof(stack));

    std::cout << "CPU Reset: PC at 0x200, Registers cleared." << std::endl;
}

void CPU::cycle() {
    // --- 1. FETCH ---
    // Read 2 bytes from memory and combine them into a 16-bit opcode
    uint16_t opcode = memory.readOpcode(PC);

    // --- 2. DECODE ---
    // Extract variables from the opcode using our masks
    uint16_t nnn = opcode & MASK_NNN;
    uint8_t  x   = (opcode & MASK_X) >> 8;
    uint8_t  y   = (opcode & MASK_Y) >> 4;
    uint8_t  kk  = opcode & MASK_KK;
    uint8_t  n   = opcode & MASK_N;

    // Increment PC by 2 (move to the next instruction)
    PC += 2;

    // --- 3. EXECUTE ---
    // The first hex digit (nibble) determines the instruction type
    switch (opcode & 0xF000) {
        case 0x0000:
            if (opcode == 0x00E0) {
                // 00E0: CLS (Clear Screen)
                // We will call display.clear() in main.cpp
            } else if (opcode == 0x00EE) {
                // 00EE: RET (Return from Subroutine)
                if (stackPointer > 0) {
                    stackPointer--;
                    PC = stack[stackPointer];
                }
            }
            break;

        case 0x1000: // 1nnn: JP addr (Jump to nnn)
            PC = nnn;
            break;

        case 0x2000: // 2nnn: CALL addr (Call subroutine)
            if (stackPointer < 16) {
                stack[stackPointer] = PC;
                stackPointer++;
                PC = nnn;
            }
            break;

        case 0x3000: // 3xkk: SE Vx, byte (Skip if Vx == kk)
            if (V[x] == kk) PC += 2;
            break;

        case 0x6000: // 6xkk: LD Vx, byte (Set Vx = kk)
            V[x] = kk;
            break;

        case 0x7000: // 7xkk: ADD Vx, byte (Set Vx = Vx + kk)
            V[x] += kk;
            break;

        case 0xA000: // Annn: LD I, addr (Set I = nnn)
            I = nnn;
            break;

        case 0xD000: // Dxy n: DRW Vx, Vy, nibble (Draw Sprite)
            // This is where the emulator draws to the screen!
            // Copilot can help you write the bit-checking logic for this.
            break;

        default:
            // std::cerr << "Unknown Opcode: 0x" << std::hex << opcode << std::endl;
            break;
    }
}

void CPU::updateTimers() {
    if (delayTimer > 0) delayTimer--;
    if (soundTimer > 0) soundTimer--;
}

} // namespace core
