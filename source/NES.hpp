#ifndef NES_HPP
#define NES_HPP

#include "CPU.hpp"
#include "Memory.hpp"

/**
 * Interface for all NES emulation.
 */
class NES
{
public:
	NES();

	Memory& getMemory();

private:
	CPU cpu;
	Memory memory;
};

#endif // NES_HPP
