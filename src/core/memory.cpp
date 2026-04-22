#include "memory.h"
#include <fstream>
#include <iostream>
#include <cstring> // for memset

namespace core {

// Hardcoded CHIP-8 font data (0 through F)
const uint8_t Memory::fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Memory::Memory() {
    initialize();
}

void Memory::initialize() {
    // Clear all RAM to 0
    std::memset(ram, 0, sizeof(ram));

    // Load fontset into memory starting at 0x050
    // (Historically, fonts were loaded from 0x000 to 0x1FF, but 0x050 is a common convention)
    for (int i = 0; i < 80; ++i) {
        ram[0x050 + i] = fontset[i];
    }
}

uint8_t Memory::readByte(uint16_t address) const {
    if (address < 4096) {
        return ram[address];
    }
    return 0; // Return 0 if out of bounds (Copilot might suggest throwing an exception here instead)
}

void Memory::writeByte(uint16_t address, uint8_t value) {
    if (address < 4096) {
        ram[address] = value;
    }
}

uint16_t Memory::readOpcode(uint16_t address) const {
    // CHIP-8 opcodes are 16-bit, stored Big-Endian.
    // Example: To read 0xA2F0, we read 0xA2, shift it left by 8 bits, and bitwise OR it with 0xF0.
    if (address < 4095) {
        return (ram[address] << 8) | ram[address + 1];
    }
    return 0;
}

bool Memory::loadROM(const std::string& filepath) {
    // Open the file as a binary stream and start at the end to get the file size
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Failed to open ROM: " << filepath << std::endl;
        return false;
    }

    // Get the size of the file and allocate a buffer
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // CHIP-8 programs start at memory address 0x200 (512 in decimal)
    // We must ensure the ROM fits into the remaining 3584 bytes of memory
    if (size > (4096 - 0x200)) {
        std::cerr << "ROM is too large to fit in memory!" << std::endl;
        return false;
    }

    // Read the file directly into the memory array starting at address 0x200
    if (file.read(reinterpret_cast<char*>(&ram[0x200]), size)) {
        return true;
    }

    return false;
}

} // namespace core
