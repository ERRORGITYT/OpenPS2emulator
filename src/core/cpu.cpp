#include <cstdint>
#include <iostream>
#include <array>
#include <iomanip>

// ... [rest of constants and CPU struct, unchanged] ...

// Logging macro for debugging
#define LOG_OPCODE(cpu, opcode) \
    std::cout << "PC: 0x" << std::hex << cpu.PC-2 << ", OP: 0x" << std::setw(4) << std::setfill('0') << opcode << std::dec << "\n"

// Fetches a 16-bit opcode, safely
uint16_t fetch_opcode(CPU& cpu) {
    if (cpu.PC + 1 >= MEMORY_SIZE) {
        std::cerr << "PC out of bounds: 0x" << std::hex << cpu.PC << std::dec << "\n";
        return 0;
    }
    uint16_t opcode = (cpu.memory[cpu.PC] << 8) | cpu.memory[cpu.PC + 1];
    cpu.PC += 2;
    return opcode;
}

// Decodes given opcode into components
void decode_opcode(uint16_t opcode, uint16_t& nnn, uint8_t& x, 
                   uint8_t& y, uint8_t& kk, uint8_t& n) {
    nnn = MASK_NNN(opcode);
    x = MASK_X(opcode);
    y = MASK_Y(opcode);
    kk = MASK_KK(opcode);
    n = MASK_N(opcode);
}

// Implements a partial CHIP-8 instruction set, extend as needed
void execute_cycle(CPU& cpu) {
    uint16_t opcode = fetch_opcode(cpu);

    // Optionally add debug log
    LOG_OPCODE(cpu, opcode);

    uint16_t nnn;
    uint8_t x, y, kk, n;
    decode_opcode(opcode, nnn, x, y, kk, n);

    switch (opcode & 0xF000) {
        case 0x0000:
            if (opcode == 0x00E0) {
                // CLS: clear the display
                // TODO: Implement screen clear
            } else if (opcode == 0x00EE) {
                // RET: return from subroutine
                if (cpu.sp == 0) {
                    std::cerr << "Stack underflow on RET\n";
                    break;
                }
                cpu.sp--;
                cpu.PC = cpu.stack[cpu.sp];
            }
            break;
        case 0x1000:
            // JP addr
            cpu.PC = nnn;
            break;
        case 0x2000:
            // CALL addr
            if (cpu.sp >= STACK_DEPTH) {
                std::cerr << "Stack overflow on CALL\n";
                break;
            }
            cpu.stack[cpu.sp++] = cpu.PC;
            cpu.PC = nnn;
            break;
        case 0x6000:
            // LD Vx, byte
            cpu.V[x] = kk;
            break;
        case 0x7000:
            // ADD Vx, byte
            cpu.V[x] += kk;
            break;
        // Extend with more CHIP-8 instructions as needed
        default:
            std::cerr << "Unknown opcode: 0x" << std::hex << opcode << std::dec << "\n";
            break;
    }
}

void update_timers(CPU& cpu) {
    if (cpu.DT > 0) cpu.DT--;
    if (cpu.ST > 0) {
        cpu.ST--;
        // Hook for sound: play beep if needed
        std::cout << "BEEP!\n";
    }
}
