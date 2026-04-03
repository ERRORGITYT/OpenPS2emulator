#include <cstdint> // Required for uint8_t and int16_t
#include <iostream>

// Paste the #define mask lines here
///Some helper functions to do common bit operations in chip8
#define mask_nnn(o) (o & 0x0fff)         ///Masks the lower 3 nibbles
#define mask_n(o) (o & 0x0f)             ///Masks the lower nibble
#define mask_xh(o) ((o & 0xf000) >> 12)  ///Masks the top most nibble of high byte
#define mask_xl(o) ((o & 0x0f00) >> 8)   ///Masks the lower nibble of high byte
#define mask_yh(o) ((o & 0x00f0) >> 4)   ///Masks the higher nibble of low byte
#define mask_yl(o) (o & 0x000f)          ///Masks the lower nibble of low byte
#define mask_high(o) ((o & 0xff00) >> 8) ///Masks the high byte
#define mask_low(o) (o & 0x00ff)         ///Masks the lower byte</code></pre>
// Paste the Memory array here
  uint8_t V[16];

  ///The chip8 has a stack space for 16 16-bit addresses.
  int16_t Stack[16];

  ///The 16-bit stack pointer is used to point to the top of the Stack space.
  int16_t SP;

  ///A 16 bit general purpose register used to store memory addresses. Only 12
  ///bits are actually used.
  int16_t I;

  ///The Program Counter is an internal register and can't be used by chip8 programs.
  int16_t PC;

  ///These 8 bit registers are used as timers. They are auto-decremented @ 60Hz,
  ///when they are non-zero. When ST is non-zero, the chip8 produces a 'tone'.
  ///NOTE: These registers are to be auto-decremented *external* to the chip8.
  uint8_t DT, ST;
// Paste the Registers (V, Stack, SP, I, PC, etc.) here
  // Big-endian order
  int16_t opcode = (Memory[PC] &lt;&lt; 8) | Memory[PC + 1];
  PC += 2;
  // cache common operations
  int16_t nnn = mask_nnn(opcode);
  uint8_t xh = mask_xh(opcode);
  uint8_t x = mask_xl(opcode);
  uint8_t y = mask_yh(opcode);
  uint8_t kk = mask_low(opcode);
  uint8_t n = mask_n(opcode);
