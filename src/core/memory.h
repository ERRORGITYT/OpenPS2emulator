#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <string>

namespace core {

class Memory {
private:
    // CHIP-8 has exactly 4096 bytes (4KB) of RAM
    uint8_t ram[4096];

    // The standard CHIP-8 fontset (Hex characters 0-F, 5 bytes each)
    static const uint8_t fontset[80];

public:
    Memory();
    ~Memory() = default;

    // Zeroes out memory and loads the fontset into the 0x000 - 0x1FF region
    void initialize();

    // Safely read a single byte
    uint8_t readByte(uint16_t address) const;

    // Safely write a single byte
    void writeByte(uint16_t address, uint8_t value);

    // Helper: Reads two consecutive bytes and combines them into a 16-bit opcode
    uint16_t readOpcode(uint16_t address) const;

    // Loads a binary game file into RAM starting at address 0x200
    bool loadROM(const std::string& filepath);
};

} // namespace core

#endif // MEMORY_H
