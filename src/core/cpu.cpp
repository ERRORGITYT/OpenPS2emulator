#include <cstdint>
#include <iostream>
#include <array>

// ============================================================================
// Constants
// ============================================================================

// Memory size for CHIP-8 (4KB)
constexpr size_t MEMORY_SIZE = 4096;

// Stack depth
constexpr size_t STACK_DEPTH = 16;

// Number of general purpose registers
constexpr size_t NUM_REGISTERS = 16;

// Program counter start address
constexpr uint16_t PROGRAM_START = 0x200;

// Timer frequency in Hz
constexpr uint16_t TIMER_FREQUENCY = 60;

// ============================================================================
// Bit Manipulation Macros
// ============================================================================

#define MASK_NNN(op) ((op) & 0x0FFF)      // Lower 12 bits (address)
#define MASK_N(op)   ((op) & 0x000F)      // Lower 4 bits (nibble)
#define MASK_X(op)   (((op) & 0x0F00) >> 8)  // Second nibble from left
#define MASK_Y(op)   (((op) & 0x00F0) >> 4)  // Third nibble from left
#define MASK_KK(op)  ((op) & 0x00FF)      // Lower byte

// ============================================================================
// CPU State Structure
// ============================================================================

struct CPU {
    // Memory (4KB)
    std::array<uint8_t, MEMORY_SIZE> memory{};
    
    // 16 general purpose 8-bit registers (V0-VF)
    // VF is used as a flag register and should not be modified by most instructions
    std::array<uint8_t, NUM_REGISTERS> V{};
    
    // Stack for subroutine calls (16 16-bit addresses)
    std::array<uint16_t, STACK_DEPTH> stack{};
    
    // Stack pointer (points to next free slot)
    uint16_t sp = 0;
    
    // Index register (12-bit, but stored in 16-bit)
    uint16_t I = 0;
    
    // Program counter (starts at 0x200)
    uint16_t PC = PROGRAM_START;
    
    // Delay timer (decremented at 60Hz)
    uint8_t DT = 0;
    
    // Sound timer (decremented at 60Hz, produces tone when non-zero)
    uint8_t ST = 0;
};

// ============================================================================
// Fetch-Decode-Execute Cycle
// ============================================================================

/**
 * Fetches the next opcode from memory and advances the program counter
 * @param cpu Reference to the CPU state
 * @return 16-bit opcode in big-endian format
 */
uint16_t fetch_opcode(CPU& cpu) {
    uint16_t opcode = (cpu.memory[cpu.PC] << 8) | cpu.memory[cpu.PC + 1];
    cpu.PC += 2;
    return opcode;
}

/**
 * Decodes the opcode and extracts instruction components
 * @param opcode The 16-bit opcode
 * @param nnn Output: 12-bit address
 * @param x Output: Register index X
 * @param y Output: Register index Y
 * @param kk Output: 8-bit constant
 * @param n Output: 4-bit constant
 */
void decode_opcode(uint16_t opcode, uint16_t& nnn, uint8_t& x, 
                   uint8_t& y, uint8_t& kk, uint8_t& n) {
    nnn = MASK_NNN(opcode);
    x = MASK_X(opcode);
    y = MASK_Y(opcode);
    kk = MASK_KK(opcode);
    n = MASK_N(opcode);
}

/**
 * Executes a single CPU cycle
 * @param cpu Reference to the CPU state
 */
void execute_cycle(CPU& cpu) {
    // Fetch
    uint16_t opcode = fetch_opcode(cpu);
    
    // Decode
    uint16_t nnn;
    uint8_t x, y, kk, n;
    decode_opcode(opcode, nnn, x, y, kk, n);
    
    // Execute (placeholder - implement instruction set here)
    // TODO: Implement CHIP-8 instruction set
}

/**
 * Updates timers (should be called at 60Hz)
 * @param cpu Reference to the CPU state
 */
void update_timers(CPU& cpu) {
    if (cpu.DT > 0) {
        cpu.DT--;
    }
    if (cpu.ST > 0) {
        cpu.ST--;
        // TODO: Produce sound when ST > 0
    }
} 
