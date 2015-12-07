#ifndef NES_HPP
#define NES_HPP

#include "CPU.hpp"
#include "Memory.hpp"
#include "PPU.hpp"
#include "ROMImage.hpp"

/**
 * Interface for all NES emulation.
 */
class NES
{
public:
	NES( uint8_t* romData );

	CPU& getCPU();
	Memory& getMemory();
	PPU& getPPU();
	ROMImage& getROMImage();

	/**
	 * Step a single frame of emulation.
	 */
	void stepFrame();

private:
	ROMImage romImage;
	Memory memory;
	CPU cpu;
	PPU ppu;
};

#endif // NES_HPP
