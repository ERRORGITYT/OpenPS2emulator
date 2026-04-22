#include "memory.h"
#include <fstream>
#include <iostream>
#include <vector>

namespace core {

Memory::Memory() {
    // Clear memory (4096 bytes)
    for (int i = 0; i < 4096; ++i) {
        ram[i] = 0;
    }
}

bool Memory::loadROM(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Failed to open ROM: " << filename << std::endl;
        return false;
    }

    std::streamsize size = file.tellg();
    
    // CHIP-8 Memory Map: 0x000 to 0x1FF is reserved for the interpreter.
    // Max ROM size is 4096 - 512 = 3584 bytes.
    if (size > (4096 - 512)) {
        std::cerr << "Error: ROM is too large (" << size << " bytes). Max allowed is 3584 bytes." << std::endl;
        return false;
    }

    file.seekg(0, std::ios::beg);
    
    // Load the ROM into RAM starting at address 0x200 (512)
    if (file.read(reinterpret_cast<char*>(&ram[512]), size)) {
        std::cout << "Successfully loaded " << size << " bytes into memory at 0x200." << std::endl;
        return true;
    }

    return false;
}

uint8_t Memory::readByte(uint16_t address) {
    return ram[address & 0xFFF]; // Masking to stay within 4096 bytes
}

uint16_t Memory::readOpcode(uint16_t address) {
    // CHIP-8 opcodes are 2 bytes long, stored Big-Endian
    uint8_t highByte = ram[address & 0xFFF];
    uint8_t lowByte = ram[(address + 1) & 0xFFF];
    return (highByte << 8) | lowByte;
}

} // namespace core
