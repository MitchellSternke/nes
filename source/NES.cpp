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

Controller& NES::getController1()
{
	return controller1;
}

Controller& NES::getController2()
{
	return controller2;
}

CPU& NES::getCPU()
{
	return cpu;
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

void NES::stepFrame()
{
	int startFrame = ppu.getFrame();
	while( startFrame == ppu.getFrame() )
	{
		// Step the CPU
		int cpuCycles = cpu.step();

		// Step the PPU
		int ppuCycles = 3 * cpuCycles;
		for( int i = 0; i < ppuCycles; i++ )
		{
			ppu.step();
		}
	}
}
