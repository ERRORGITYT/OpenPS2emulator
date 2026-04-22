#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <mutex>
#include <memory>

namespace core {

/**
 * @class Memory
 * @brief High-performance virtualized memory controller for CHIP-8/SCHIP.
 * Handles dual font-sets, ROM injection, and state persistence.
 */
class Memory {
public:
    // --- Lifecycle ---
    Memory();
    ~Memory() = default;

    // --- Core Bus Operations ---
    /**
     * @brief Reads a single byte from the 4KB address space.
     * Includes bounds checking via bitmasking.
     */
    uint8_t readByte(uint16_t address) const;

    /**
     * @brief Writes a single byte to RAM.
     * Includes write-protection for the interpreter/font-set area (0x000-0x1FF).
     */
    void writeByte(uint16_t address, uint8_t value);

    /**
     * @brief Fetches a 16-bit big-endian opcode from memory.
     */
    uint16_t readOpcode(uint16_t address) const;

    // --- ROM & State Management ---
    bool loadROM(const std::string& filename);
    bool saveState(const std::string& filename) const;
    bool loadState(const std::string& filename);

    // --- Hardware Emulation Utilities ---
    /**
     * @brief Returns a pointer to the font data for character 'c'.
     * Handles both 5-byte (CHIP8) and 10-byte (SCHIP) character sets.
     */
    uint16_t getFontAddress(uint8_t character, bool large = false) const;

    // --- Debugging & Telemetry ---
    void dumpMemory(uint16_t start, uint16_t end) const;
    void reset();

    // Constant Memory Map Offsets
    static constexpr uint16_t ROM_START_ADDR = 0x200;
    static constexpr uint16_t FONT_START_ADDR = 0x050;
    static constexpr uint16_t LARGE_FONT_ADDR = 0x0A0;
    static constexpr uint16_t RAM_SIZE       = 4096;

private:
    // The physical 4KB RAM buffer
    std::array<uint8_t, RAM_SIZE> ram;

    // Mutex for thread-safe access (Helio G85 multi-core safety)
    mutable std::mutex busMutex;

    // Internal helper for address safety
    inline uint16_t mask(uint16_t address) const { return address & 0xFFF; }
};

} // namespace core

#endif // MEMORY_H
