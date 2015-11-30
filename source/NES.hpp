#ifndef NES_HPP
#define NES_HPP

#include "CPU.hpp"
#include "Memory.hpp"
#include "ROMImage.hpp"

/**
 * Interface for all NES emulation.
 */
class NES
{
public:
	NES( uint8_t* romData );

	Memory& getMemory();
	ROMImage& getROMImage();

private:
	ROMImage romImage;
	CPU cpu;
	Memory memory;
};

#endif // NES_HPP
