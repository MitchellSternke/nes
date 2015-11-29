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
	// RAM and Mirrors
	if( address < 0x2000 )
	{
		return ram[address & 0x7ff];
	}
	// PPU Registers and Mirrors
	else if( address < 0x4000 )
	{
		///@todo implement
	}
	// APU and I/O registers
	else if( address < 0x4020 )
	{
		///@todo implement
	}
	// Cartridge space
	else
	{
		///@todo implement
	}

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
