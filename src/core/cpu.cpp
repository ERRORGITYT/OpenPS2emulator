#include "cpu.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <random>

namespace core {

// Instruction Deconstruction Masks
constexpr uint16_t MASK_NNN  = 0x0FFF; // Address
constexpr uint16_t MASK_X    = 0x0F00; // Register X
constexpr uint16_t MASK_Y    = 0x00F0; // Register Y
constexpr uint16_t MASK_KK   = 0x00FF; // 8-bit Constant
constexpr uint16_t MASK_N    = 0x000F; // 4-bit Constant
constexpr uint16_t TYPE_MSB  = 0xF000; // Most Significant Nibble

CPU::CPU(Memory& mem) : memory(mem) {
    // Seed the Mersenne Twister RNG with a real hardware entropy source
    std::random_device rd;
    rng.seed(rd());
    
    // Set distribution range for 8-bit values (0-255)
    dist = std::uniform_int_distribution<uint8_t>(0, 255);
    
    reset();
}

void CPU::reset() {
    PC = 0x200; // CHIP-8 entry point
    I = 0;
    stackPointer = 0;
    delayTimer = 0;
    soundTimer = 0;
    
    std::memset(V, 0, sizeof(V));
    std::memset(stack, 0, sizeof(stack));

    std::cout << "[CPU] Hardware State Reset. PC: 0x200. Registers Zeroed." << std::endl;
}

void CPU::cycle() {
    // 1. FETCH: Opcodes are 16-bit, big-endian
    uint16_t opcode = memory.readOpcode(PC);
    PC += 2;

    // 2. DECODE & EXECUTE
    executeOpcode(opcode);
}

void CPU::executeOpcode(uint16_t opcode) {
    uint16_t nnn = opcode & MASK_NNN;
    uint8_t x    = (opcode & MASK_X) >> 8;
    uint8_t y    = (opcode & MASK_Y) >> 4;
    uint8_t kk   = opcode & MASK_KK;
    uint8_t n    = opcode & MASK_N;

    switch (opcode & TYPE_MSB) {
        case 0x0000:
            if (opcode == 0x00E0) { /* CLS: Logic in display.cpp */ }
            else if (opcode == 0x00EE) { // RET
                if (stackPointer > 0) {
                    stackPointer--;
                    PC = stack[stackPointer];
                }
            }
            break;

        case 0x1000: PC = nnn; break; // JP addr
        case 0x2000: // CALL addr
            stack[stackPointer] = PC;
            stackPointer++;
            PC = nnn;
            break;

        case 0x3000: if (V[x] == kk) PC += 2; break; // SE Vx, byte
        case 0x4000: if (V[x] != kk) PC += 2; break; // SNE Vx, byte
        case 0x6000: V[x] = kk; break;               // LD Vx, byte
        case 0x7000: V[x] += kk; break;              // ADD Vx, byte

        case 0x8000: // --- THE ARITHMETIC ENGINE ---
            switch (n) {
                case 0x0: V[x] = V[y]; break; // LD Vx, Vy
                case 0x1: V[x] |= V[y]; break; // OR Vx, Vy
                case 0x2: V[x] &= V[y]; break; // AND Vx, Vy
                case 0x3: V[x] ^= V[y]; break; // XOR Vx, Vy
                case 0x4: { // ADD Vx, Vy (with Carry)
                    uint16_t sum = V[x] + V[y];
                    V[0xF] = (sum > 255) ? 1 : 0;
                    V[x] = sum & 0xFF;
                } break;
                case 0x5: // SUB Vx, Vy
                    V[0xF] = (V[x] > V[y]) ? 1 : 0;
                    V[x] -= V[y];
                    break;
                case 0x6: // SHR Vx
                    V[0xF] = V[x] & 0x1;
                    V[x] >>= 1;
                    break;
                case 0xE: // SHL Vx
                    V[0xF] = (V[x] & 0x80) >> 7;
                    V[x] <<= 1;
                    break;
            }
            break;

        case 0xA000: I = nnn; break; // LD I, addr
        
        case 0xC000: // RND Vx, byte (THE RNG CORE)
            V[x] = dist(rng) & kk; 
            break;

        case 0xD000: // DRW Vx, Vy, nibble
            // Collision detection and sprite drawing logic
            break;

        case 0xF000: // --- SYSTEM UTILITIES ---
            switch (kk) {
                case 0x07: V[x] = delayTimer; break;
                case 0x15: delayTimer = V[x]; break;
                case 0x18: soundTimer = V[x]; break;
                case 0x1E: I += V[x]; break;
                case 0x29: I = 0x050 + (V[x] * 5); break;  // Point to 5-byte font
                case 0x30: I = 0x0A0 + (V[x] * 10); break; // Point to 10-byte font
                case 0x33: // BCD Storage
                    memory.writeByte(I, V[x] / 100);
                    memory.writeByte(I + 1, (V[x] / 10) % 10);
                    memory.writeByte(I + 2, V[x] % 10);
                    break;
                case 0x55: // Store registers V0-Vx in memory
                    for (int i = 0; i <= x; i++) memory.writeByte(I + i, V[i]);
                    break;
                case 0x65: // Load registers V0-Vx from memory
                    for (int i = 0; i <= x; i++) V[i] = memory.readByte(I + i);
                    break;
            }
            break;

        default:
            // std::cerr << "[CPU] UNKNOWN OPCODE: 0x" << std::hex << opcode << std::endl;
            break;
    }
}

void CPU::updateTimers() {
    if (delayTimer > 0) delayTimer--;
    if (soundTimer > 0) soundTimer--;
}

} // namespace core
