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
	~PPU();

	/**
	 * Get the current frame number.
	 */
	int getFrame() const;

	/**
	 * Get the rendered frame buffer.
	 */
	const uint32_t* getFrameBuffer() const;

	/**
	 * Get an ARGB representation of the nametable.
	 */
	uint32_t* getVisualNametable();

	/**
	 * Get an ARGB representation of the palette.
	 */
	uint32_t* getVisualPalette();

	/**
	 * Get an ARGB representation of the pattern table.
	 */
	uint32_t* getVisualPatternTable();

	/**
	 * Read a PPU register value.
	 */
	uint8_t readRegister( uint16_t address );

	/**
	 * Step the PPU emulation by one cycle.
	 */
	void step();

	/**
	 * Have the PPU perform a DMA transfer.
	 */
	void writeDMA( uint8_t page );

	/**
	 * Write to a PPU register.
	 */
	void writeRegister( uint16_t address, uint8_t value );

private:
	/**
	 * Mirror mode for the NameTable.
	 */
	enum NametableMirrorMode
	{
		MIRROR_HORIZONTAL = 0,
		MIRROR_VERTICAL   = 1,
	};

	/**
	 * PPU Registers that can be directly read/written to.
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
			Bit<7>  nmiEnable;
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
	};

	//*****************************************************************
	// Member variables
	//*****************************************************************

	NES& nes;
	Registers registers;

	uint8_t oamAddress; /**< $2003 (OAMADDR) */

	// Memory
	uint8_t palette[32];     /**< 32 bytes of palette data. */
	uint8_t nametable[2048]; /**< 2kb nametable data. */
	uint8_t oam[256];

	// PPU Address control
	Word currentAddress; /**< The current address that will be accessed on the next PPU read/write. */
	bool writeToggle;    /**< Toggles whether the low or high bit of the current address will be set on the next write to PPUADDR. */

	// Timing Counters
	int frame;    /**< The current frame number. */
	int scanline; /**< The scanline number of the current frame. */
	int cycle;    /**< The cycle number of the current scanline. */

	// Framebuffer
	uint32_t* framebuffer[2]; /**< Rendered frames get drawn here. */

	//*****************************************************************
	// Private Methods
	//*****************************************************************

	/**
	 * Get the value of the attribute table for a given tile in a nametable.
	 */
	uint8_t getAttributeTableValue( uint16_t nametableAddress );

	/**
	 * Convert a nametable address to an index of the nametable.
	 */
	uint16_t getNametableIndex( uint16_t address );

	/**
	 * Read a byte from the PPU address space.
	 */
	uint8_t readByte( uint16_t address );

	/**
	 * Read from the PPUDATA register.
	 */
	uint8_t readDataRegister();

	/**
	 * Render a frame to the framebuffer.
	 */
	void renderFrame();

	/**
	 * Write to PPUADDR register.
	 */
	void writeAddressRegister( uint8_t value );

	/**
	 * Write a byte to the PPU address space.
	 */
	void writeByte( uint16_t address, uint8_t value );

	/**
	 * Write to PPUDATA register.
	 */
	void writeDataRegister( uint8_t value );
};

#endif // PPU_HPP
