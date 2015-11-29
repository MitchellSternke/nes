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

private:
	CPU cpu;
	Memory memory;
	ROMImage romImage;
};

#endif // NES_HPP
