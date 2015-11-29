#include "NES.hpp"

NES::NES( uint8_t* romData ) :
	cpu(*this),
	memory(*this),
	romImage(romData)
{
	romImage.getHeader()->print();
}

Memory& NES::getMemory()
{
	return memory;
}
