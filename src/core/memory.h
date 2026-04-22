#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <array>
#include <string>
#include <mutex>

namespace core {

class Memory {
public:
    Memory();
    ~Memory() = default;

    // Core Operations (Must be 'const' to allow reading without changing state)
    uint8_t readByte(uint16_t address) const;
    uint16_t readOpcode(uint16_t address) const;
    void writeByte(uint16_t address, uint8_t value);

    // ROM & State
    bool loadROM(const std::string& filename);
    bool saveState(const std::string& filename) const;
    bool loadState(const std::string& filename);

private:
    std::array<uint8_t, 4096> ram;
    
    // The 'mutable' keyword allows the mutex to be locked inside 'const' functions
    mutable std::mutex memMutex; 
};

} // namespace core

#endif
