#ifndef MEMORY_H
#define MEMORY_H

// --- THE HEAVY INCLUDES ---
#include <cstdint>      // Standard integer types (uint8_t, etc.)
#include <string>       // String handling for file paths
#include <array>        // Fixed-size containers for RAM
#include <vector>       // Dynamic arrays for ROM buffering
#include <fstream>      // File stream for loading ROMs
#include <iostream>     // Console output for debugging
#include <memory>       // Smart pointers (unique_ptr, shared_ptr)
#include <mutex>        // Thread safety for memory access
#include <algorithm>    // Search, sort, and copy algorithms
#include <iomanip>      // Hexadecimal formatting for logs

namespace core {

class Memory {
public:
    Memory();
    ~Memory() = default;

    // Core Functions
    bool loadROM(const std::string& filename);
    uint8_t readByte(uint16_t address) const;
    void writeByte(uint16_t address, uint8_t value);
    uint16_t readOpcode(uint16_t address) const;

    // Debugging Tools
    void dumpMemory(uint16_t start, uint16_t end) const;

private:
    std::array<uint8_t, 4096> ram;
    mutable std::mutex memMutex; // Prevents simultaneous read/writes
};

} // namespace core

#endif
