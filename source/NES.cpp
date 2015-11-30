#include "Mapper.hpp"
#include "NES.hpp"

NES::NES( uint8_t* romData ) :
	romImage(romData),
	cpu(*this),
	memory(*this)
{
	romImage.getHeader()->print();
	memory.getMapper().print();
}

Memory& NES::getMemory()
{
	return memory;
}

ROMImage& NES::getROMImage()
{
	return romImage;
}
