#include "Mapper.hpp"
#include "NES.hpp"

NES::NES( uint8_t* romData ) :
	romImage(romData),
	memory(*this),
	cpu(*this),
	ppu(*this)
{
	romImage.getHeader()->print();
	memory.getMapper().print();
}

Memory& NES::getMemory()
{
	return memory;
}

PPU& NES::getPPU()
{
	return ppu;
}

ROMImage& NES::getROMImage()
{
	return romImage;
}

void NES::run()
{
	while(true)
	{
		int cpuCycles = cpu.step();
	}
}
