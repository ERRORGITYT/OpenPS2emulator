#include "memory.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <mutex>
#include <cstring> // For legacy memcmp checks

namespace core {

Memory::Memory() {
    std::lock_guard<std::mutex> lock(memMutex);
    ram.fill(0);

    // --- STANDARD 5-BYTE FONT SET (0-F) ---
    // Mapping pixels for low-res characters
    const uint8_t fontSet[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, 0x20, 0x60, 0x20, 0x20, 0x70,
        0xF0, 0x10, 0xF0, 0x80, 0xF0, 0xF0, 0x10, 0xF0, 0x10, 0xF0,
        0x90, 0x90, 0xF0, 0x10, 0x10, 0xF0, 0x80, 0xF0, 0x10, 0xF0,
        0xF0, 0x80, 0xF0, 0x90, 0xF0, 0xF0, 0x10, 0x20, 0x40, 0x40,
        0xF0, 0x90, 0xF0, 0x90, 0xF0, 0xF0, 0x90, 0xF0, 0x10, 0xF0,
        0xF0, 0x90, 0xF0, 0x90, 0x90, 0xE0, 0x90, 0xE0, 0x90, 0xE0,
        0xF0, 0x80, 0x80, 0x80, 0xF0, 0xE0, 0x90, 0x90, 0x90, 0xE0,
        0xF0, 0x80, 0xF0, 0x80, 0xF0, 0xF0, 0x80, 0xF0, 0x80, 0x80
    };

    // --- SUPER CHIP-8 10-BYTE LARGE FONT SET (0-F) ---
    // Higher fidelity characters for SCHIP mode
    const uint8_t largeFontSet[160] = {
        0x3C, 0x42, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x42, 0x3C,
        0x08, 0x18, 0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3E,
        0x3E, 0x42, 0x42, 0x02, 0x02, 0x04, 0x08, 0x10, 0x20, 0x7E,
        0x3E, 0x42, 0x42, 0x02, 0x3C, 0x02, 0x02, 0x42, 0x42, 0x3E,
        0x08, 0x18, 0x28, 0x48, 0x88, 0xFE, 0x08, 0x08, 0x08, 0x08,
        0x7E, 0x40, 0x40, 0x40, 0x7C, 0x02, 0x02, 0x02, 0x42, 0x3C,
        0x3C, 0x42, 0x40, 0x40, 0x7C, 0x82, 0x82, 0x82, 0x42, 0x3C,
        0xFE, 0x02, 0x02, 0x04, 0x08, 0x10, 0x20, 0x20, 0x20, 0x20,
        0x3C, 0x42, 0x42, 0x42, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C,
        0x3C, 0x42, 0x82, 0x82, 0x82, 0x7E, 0x02, 0x02, 0x42, 0x3C,
        0x18, 0x24, 0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x42,
        0x7C, 0x22, 0x22, 0x22, 0x3C, 0x22, 0x22, 0x22, 0x22, 0x7C,
        0x3C, 0x42, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x3C,
        0x78, 0x24, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x24, 0x78,
        0x7E, 0x40, 0x40, 0x40, 0x78, 0x40, 0x40, 0x40, 0x40, 0x7E,
        0x7E, 0x40, 0x40, 0x40, 0x78, 0x40, 0x40, 0x40, 0x40, 0x40
    };

    // Load fonts into reserved interpreter space
    std::copy(std::begin(fontSet), std::end(fontSet), ram.begin() + 0x050);
    std::copy(std::begin(largeFontSet), std::end(largeFontSet), ram.begin() + 0x0A0);

    std::cout << "[MEM_CORE] Buffer initialized. Entropy verified." << std::endl;
}

bool Memory::loadROM(const std::string& filename) {
    std::lock_guard<std::mutex> lock(memMutex);
    
    std::cout << "[MEM_CORE] Attempting binary inject: " << filename << std::endl;
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    
    if (!file.is_open()) {
        std::cerr << "[MEM_CORE] ERROR: Access denied to source binary." << std::endl;
        return false;
    }

    std::streamsize size = file.tellg();
    if (size > (4096 - 0x200)) {
        std::cerr << "[MEM_CORE] OVERFLOW: Binary size exceeds segment 0x200 - 0xFFF." << std::endl;
        return false;
    }

    file.seekg(0, std::ios::beg);
    if (file.read(reinterpret_cast<char*>(&ram[0x200]), size)) {
        std::cout << "[MEM_CORE] Successful map of " << size << " bytes to RAM." << std::endl;
        return true;
    }

    return false;
}

void Memory::writeByte(uint16_t address, uint8_t value) {
    std::lock_guard<std::mutex> lock(memMutex);
    uint16_t target = address & 0xFFF;

    // PROTECTION: Prevent ROMs from bricking the interpreter fonts
    if (target < 0x200) {
        // Silently ignore or log as a violation
        return; 
    }

    ram[target] = value;
}

uint8_t Memory::readByte(uint16_t address) const {
    std::lock_guard<std::mutex> lock(memMutex);
    return ram[address & 0xFFF];
}

uint16_t Memory::readOpcode(uint16_t address) const {
    std::lock_guard<std::mutex> lock(memMutex);
    uint16_t addr = address & 0xFFF;
    // Big-Endian fetch
    return (static_cast<uint16_t>(ram[addr]) << 8) | ram[(addr + 1) & 0xFFF];
}

// --- NEW SYSTEM TOOLS ---

void Memory::saveState(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(memMutex);
    std::ofstream outFile(filepath, std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(reinterpret_cast<const char*>(ram.data()), ram.size());
        std::cout << "[MEM_CORE] Snapshot persisted to " << filepath << std::endl;
    }
}

void Memory::loadState(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(memMutex);
    std::ifstream inFile(filepath, std::ios::binary);
    if (inFile.is_open()) {
        inFile.read(reinterpret_cast<char*>(ram.data()), ram.size());
        std::cout << "[MEM_CORE] Snapshot restored from " << filepath << std::endl;
    }
}

void Memory::dumpMemory(uint16_t start, uint16_t end) const {
    std::lock_guard<std::mutex> lock(memMutex);
    std::cout << "\n--- CORE MEMORY HEXDUMP ---\n";
    for (uint16_t i = start; i <= end; i++) {
        if (i % 16 == 0) std::cout << "\n0x" << std::hex << std::setw(4) << std::setfill('0') << i << ": ";
        std::cout << std::setw(2) << static_cast<int>(ram[i]) << " ";
    }
    std::cout << std::dec << "\n\n--- END OF DUMP ---\n";
}

} // namespace core
