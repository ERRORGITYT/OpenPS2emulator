#include "cpu.h"
#include <iostream>
#include <cstring>
#include <iomanip>

namespace core {

// Instruction Masks
constexpr uint16_t MASK_NNN = 0x0FFF;
constexpr uint16_t MASK_X   = 0x0F00;
constexpr uint16_t MASK_Y   = 0x00F0;
constexpr uint16_t MASK_KK  = 0x00FF;
constexpr uint16_t MASK_N   = 0x000F;

CPU::CPU(Memory& mem) : memory(mem), dist(0, 255) {
    std::random_device rd;
    rng.seed(rd());
    reset();
}

void CPU::reset() {
    PC = 0x200;
    I = 0;
    stackPointer = 0;
    delayTimer = 0;
    soundTimer = 0;
    std::memset(V, 0, sizeof(V));
    std::memset(stack, 0, sizeof(stack));
    std::cout << "[CPU] Core Reset. Ready for Binary Execution." << std::endl;
}

void CPU::cycle() {
    uint16_t opcode = memory.readOpcode(PC);
    
    // Log instruction for debugging (The Real Spam)
    // std::cout << "Exec: 0x" << std::hex << opcode << " at PC: 0x" << PC << std::dec << std::endl;

    PC += 2;
    executeOpcode(opcode);
}

void CPU::executeOpcode(uint16_t opcode) {
    uint16_t nnn = opcode & MASK_NNN;
    uint8_t  x   = (opcode & MASK_X) >> 8;
    uint8_t  y   = (opcode & MASK_Y) >> 4;
    uint8_t  kk  = opcode & MASK_KK;
    uint8_t  n   = opcode & MASK_N;

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode) {
                case 0x00E0: /* Clear Screen Call */ break;
                case 0x00EE: // RET
                    if (stackPointer > 0) {
                        stackPointer--;
                        PC = stack[stackPointer];
                    }
                    break;
                case 0x00FF: // SCHIP: Enable High-Res (128x64)
                    std::cout << "[CPU] Mode: SCHIP High-Res Enabled" << std::endl;
                    break;
                default: break;
            }
            break;

        case 0x1000: PC = nnn; break;      // 1nnn: JP addr
        case 0x2000:                       // 2nnn: CALL addr
            stack[stackPointer] = PC;
            stackPointer++;
            PC = nnn;
            break;

        case 0x3000: if (V[x] == kk) PC += 2; break; // 3xkk: SE Vx, byte
        case 0x4000: if (V[x] != kk) PC += 2; break; // 4xkk: SNE Vx, byte
        case 0x5000: if (V[x] == V[y]) PC += 2; break; // 5xy0: SE Vx, Vy

        case 0x6000: V[x] = kk; break;     // 6xkk: LD Vx, byte
        case 0x7000: V[x] += kk; break;    // 7xkk: ADD Vx, byte

        case 0x8000: // Arithmetic Engine
            switch (opcode & 0x000F) {
                case 0x0: V[x] = V[y]; break;
                case 0x1: V[x] |= V[y]; break;
                case 0x2: V[x] &= V[y]; break;
                case 0x3: V[x] ^= V[y]; break;
                case 0x4: { // ADD with Carry
                    uint16_t sum = V[x] + V[y];
                    V[0xF] = (sum > 255) ? 1 : 0;
                    V[x] = sum & 0xFF;
                } break;
                case 0x5: // SUB
                    V[0xF] = (V[x] > V[y]) ? 1 : 0;
                    V[x] -= V[y];
                    break;
                case 0x6: // SHR (Shift Right)
                    V[0xF] = V[x] & 0x1;
                    V[x] >>= 1;
                    break;
                case 0xE: // SHL (Shift Left)
                    V[0xF] = (V[x] & 0x80) >> 7;
                    V[x] <<= 1;
                    break;
            }
            break;

        case 0xA000: I = nnn; break; // Annn: LD I, addr
        case 0xC000: V[x] = dist(rng) & kk; break; // Cxkk: RND Vx, byte

        case 0xD000: // Dxy n: DISPLAY
            // For now, we'll just log that we are drawing
            break;

        case 0xF000:
            switch (kk) {
                case 0x07: V[x] = delayTimer; break;
                case 0x15: delayTimer = V[x]; break;
                case 0x18: soundTimer = V[x]; break;
                case 0x1E: I += V[x]; break;
                case 0x29: I = 0x050 + (V[x] * 5); break;  // Point I to small font
                case 0x30: I = 0x0A0 + (V[x] * 10); break; // SCHIP: Point I to large font
                case 0x33: // BCD Conversion
                    memory.writeByte(I, V[x] / 100);
                    memory.writeByte(I + 1, (V[x] / 10) % 10);
                    memory.writeByte(I + 2, V[x] % 10);
                    break;
                case 0x65: // Read registers from memory
                    for (int i = 0; i <= x; i++) V[i] = memory.readByte(I + i);
                    break;
            }
            break;

        default: break;
    }
}

void CPU::updateTimers() {
    if (delayTimer > 0) delayTimer--;
    if (soundTimer > 0) soundTimer--;
}

} // namespace core
