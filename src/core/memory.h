#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <array>
#include <string>
#include <mutex>
#include <vector>

namespace core {

class Memory {
public:
    Memory();
    ~Memory() = default;

    uint8_t  readByte(uint16_t address) const;
    uint16_t readOpcode(uint16_t address) const;
    void     writeByte(uint16_t address, uint8_t value);

    bool loadROM(const std::string& filename);
    bool saveState(const std::string& filename) const;
    bool loadState(const std::string& filename);
    void reset();

    void dump(uint16_t start, uint16_t end) const;

    static constexpr uint16_t RAM_SIZE        = 4096;
    static constexpr uint16_t ENTRY_POINT     = 0x200;
    static constexpr uint16_t FONT_BASE       = 0x050; 
    static constexpr uint16_t SCHIP_FONT_BASE = 0x0A0; 

private:
    std::array<uint8_t, RAM_SIZE> ram;
    mutable std::mutex busMutex; // Matches the .cpp file now

    void injectFonts();
};

} // namespace core

#endif
