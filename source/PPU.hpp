#ifndef PPU_HPP
#define PPU_HPP

#include "Types.hpp"

class NES;

/**
 * Emulates the Picture Processing Unit.
 */
class PPU
{
public:
	PPU(NES& nes);

	/**
	 * Read a PPU register value.
	 */
	uint8_t readByte( uint16_t address );

	/**
	 * Write to a PPU register.
	 */
	void writeByte( uint16_t address, uint8_t value );

private:
	NES& nes;
};

#endif // PPU_HPP
