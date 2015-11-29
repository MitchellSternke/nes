#include "Memory.hpp"

//*********************************************************************
// MemoryAccess wrapper
//*********************************************************************

MemoryAccess::MemoryAccess( Memory& memory, uint16_t address ) :
	memory(memory),
	address(address)
{
}

MemoryAccess& MemoryAccess::operator = ( uint8_t value )
{
	memory.writeByte(address, value);
	return *this;
}

MemoryAccess::operator uint8_t()
{
	return memory.readByte(address);
}

//*********************************************************************
// Memory class
//*********************************************************************

Memory::Memory( NES& nes ) :
	nes(nes)
{
}

uint8_t Memory::readByte( uint16_t address )
{
	///@todo implement
	return 0;
}

uint16_t Memory::readWord( uint16_t address )
{
	///@todo implement
	return 0;
}

void Memory::writeByte( uint16_t address, uint8_t value )
{
	///@todo implement
}
