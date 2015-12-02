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
	 * Step the PPU emulation by one cycle.
	 */
	void step();

	/**
	 * Write to a PPU register.
	 */
	void writeByte( uint16_t address, uint8_t value );

private:
	/**
	 * PPU Registers.
	 */
	struct Registers
	{
		// $2000
		union
		{
			uint8_t raw;
			Bit<0, 2>  nameTable;
			Bit<2>  increment;
			Bit<3>  spriteTile;
			Bit<4>  backgroundTable;
			Bit<5>  spriteHeight;
			Bit<6>  masterSlave;
		} PPUCTRL;

		// $2001
		union
		{
			uint8_t raw;
			Bit<0> grayscale;
			Bit<1> showLeftBackground;
			Bit<2> showLeftSprites;
			Bit<3> showBackground;
			Bit<4> showSprites;
			Bit<5> redTint;
			Bit<6> greenTint;
			Bit<7> blueTint;
		} PPUMASK;

		// $2002
		union
		{
			uint8_t raw;
			Bit<5> spriteOverflow;
			Bit<6> spriteZeroHit;
		} PPUSTATUS;

		// $2003
		uint8_t OAMADDR;
	};

	NES& nes;
	Registers registers;

	// Timing Counters
	int frame;    /**< The current frame number. */
	int scanline; /**< The scanline number of the current frame. */
	int cycle;    /**< The cycle number of the current scanline. */
};

#endif // PPU_HPP
