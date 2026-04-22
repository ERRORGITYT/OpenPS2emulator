#include "cpu.h"
#include <iostream>
#include <cstring>

namespace core {

CPU::CPU(Memory& mem) : memory(mem), dist(0, 255) {
    std::random_device rd;
    rng.seed(rd());
    reset();
}

void CPU::reset() {
    PC = 0x200;
    I = 0;
    sp = 0;
    delayTimer = 0;
    soundTimer = 0;
    std::memset(V, 0, sizeof(V));
    std::memset(stack, 0, sizeof(stack));
    display.fill(0);
    keys.fill(0);
}

void CPU::cycle() {
    uint16_t opcode = memory.readOpcode(PC);
    PC += 2;
    executeOpcode(opcode);
}

void CPU::executeOpcode(uint16_t opcode) {
    uint16_t nnn = opcode & 0x0FFF;
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t kk = opcode & 0x00FF;
    uint8_t n = opcode & 0x000F;

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (kk) {
                case 0xE0: display.fill(0); break; // CLS
                case 0xEE: PC = stack[--sp]; break; // RET
            }
            break;

        case 0x1000: PC = nnn; break; // JP addr
        case 0x2000: stack[sp++] = PC; PC = nnn; break; // CALL addr
        case 0x3000: if (V[x] == kk) PC += 2; break; // SE Vx, byte
        case 0x4000: if (V[x] != kk) PC += 2; break; // SNE Vx, byte
        case 0x5000: if (V[x] == V[y]) PC += 2; break; // SE Vx, Vy
        case 0x6000: V[x] = kk; break; // LD Vx, byte
        case 0x7000: V[x] += kk; break; // ADD Vx, byte

        case 0x8000: // Arithmetic Group
            switch (n) {
                case 0x0: V[x] = V[y]; break;
                case 0x1: V[x] |= V[y]; V[0xF] = 0; break; // Bitwise OR
                case 0x2: V[x] &= V[y]; V[0xF] = 0; break; // Bitwise AND
                case 0x3: V[x] ^= V[y]; V[0xF] = 0; break; // Bitwise XOR
                case 0x4: {
                    uint16_t sum = V[x] + V[y];
                    V[0xF] = (sum > 255) ? 1 : 0;
                    V[x] = sum & 0xFF;
                } break;
                case 0x5: {
                    V[0xF] = (V[x] >= V[y]) ? 1 : 0;
                    V[x] -= V[y];
                } break;
                case 0x6: V[0xF] = V[x] & 0x1; V[x] >>= 1; break; // SHR
                case 0x7: V[0xF] = (V[y] >= V[x]) ? 1 : 0; V[x] = V[y] - V[x]; break;
                case 0xE: V[0xF] = (V[x] & 0x80) >> 7; V[x] <<= 1; break; // SHL
            }
            break;

        case 0x9000: if (V[x] != V[y]) PC += 2; break;
        case 0xA000: I = nnn; break;
        case 0xB000: PC = nnn + V[0]; break;
        case 0xC000: V[x] = dist(rng) & kk; break;

        case 0xD000: { // DXYN: The Sprite Drawing Engine
            V[0xF] = 0;
            for (int row = 0; row < n; row++) {
                uint8_t spriteByte = memory.readByte(I + row);
                for (int col = 0; col < 8; col++) {
                    if ((spriteByte & (0x80 >> col)) != 0) {
                        uint16_t pixelX = (V[x] + col) % 64;
                        uint16_t pixelY = (V[y] + row) % 32;
                        uint16_t idx = pixelX + (pixelY * 64);
                        if (display[idx] == 1) V[0xF] = 1;
                        display[idx] ^= 1;
                    }
                }
            }
        } break;

        case 0xE000:
            if (kk == 0x9E) { if (keys[V[x]]) PC += 2; }
            else if (kk == 0xA1) { if (!keys[V[x]]) PC += 2; }
            break;

        case 0xF000:
            switch (kk) {
                case 0x07: V[x] = delayTimer; break;
                case 0x0A: { // Wait for key press
                    bool pressed = false;
                    for (int i = 0; i < 16; i++) {
                        if (keys[i]) { V[x] = i; pressed = true; break; }
                    }
                    if (!pressed) PC -= 2; // Loop instruction until key is pressed
                } break;
                case 0x15: delayTimer = V[x]; break;
                case 0x18: soundTimer = V[x]; break;
                case 0x1E: I += V[x]; break;
                case 0x29: I = 0x050 + (V[x] * 5); break; // Standard Font
                case 0x30: I = 0x0A0 + (V[x] * 10); break; // SCHIP Large Font
                case 0x33: // Binary Coded Decimal
                    memory.writeByte(I, V[x] / 100);
                    memory.writeByte(I + 1, (V[x] / 10) % 10);
                    memory.writeByte(I + 2, V[x] % 10);
                    break;
                case 0x55: for (int i = 0; i <= x; i++) memory.writeByte(I + i, V[i]); break;
                case 0x65: for (int i = 0; i <= x; i++) V[i] = memory.readByte(I + i); break;
            }
            break;
    }
}

void CPU::updateTimers() {
    if (delayTimer > 0) delayTimer--;
    if (soundTimer > 0) soundTimer--;
}

} // namespace core
