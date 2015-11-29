#include "NES.hpp"

NES::NES() :
	cpu(*this),
	memory(*this)
{
}

Memory& NES::getMemory()
{
	return memory;
}
