#include <iostream>

#include "Memory.hpp"
#include "NES.hpp"
#include "NROM.hpp"

//*********************************************************************
// MemoryAccess wrapper
//*********************************************************************

MemoryAccess::MemoryAccess( Memory& memory, uint16_t address ) :
	memory(memory),
	address(address),
	valueInitialized(false)
{
}

uint16_t MemoryAccess::getAddress() const
{
	return address;
}

MemoryAccess& MemoryAccess::operator = ( uint8_t value )
{
	memory.writeByte(address, value);
	valueInitialized = false;
	return *this;
}

MemoryAccess::operator uint8_t()
{
	if( !valueInitialized )
	{
		value = memory.readByte(address);
		valueInitialized = true;
	}
	return value;
}

//*********************************************************************
// Memory class
//*********************************************************************

Memory::Memory( NES& nes ) :
	nes(nes),
	mapper(nullptr)
{
	// Create the mapper
	switch( nes.getROMImage().getHeader()->getMapper() )
	{
	case 0:
		mapper = new NROM(nes);
		break;
	default:
		std::cout << "Error: unimplemented mapper number: " << (uint16_t)nes.getROMImage().getHeader()->getMapper() << std::endl;
		exit(-1);
		break;
	}
}

Mapper& Memory::getMapper()
{
	return *mapper;
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
		return nes.getPPU().readRegister(0x2000 + (address & 0x7));
	}
	// APU and I/O registers
	else if( address < 0x4020 )
	{
		switch( address )
		{
		case 0x4016:
			return nes.getController1().readByte();
		case 0x4017:
			return nes.getController2().readByte();
		default:
			break;
		}
	}
	// Cartridge space (mapper)
	else
	{
		return mapper->readByte(address);
	}

	return 0;
}

uint16_t Memory::readWord( uint16_t address )
{
	return (uint16_t)readByte(address) | ((uint16_t)readByte(address + 1) << 8);
}

void Memory::writeByte( uint16_t address, uint8_t value )
{
	// RAM and Mirrors
	if( address < 0x2000 )
	{
		ram[address & 0x7ff] = value;
	}
	// PPU Registers and Mirrors
	else if( address < 0x4000 )
	{
		nes.getPPU().writeRegister(0x2000 + (address & 0x7), value);
	}
	// APU and I/O registers
	else if( address < 0x4020 )
	{
		switch( address )
		{
		case 0x4014:
			nes.getPPU().writeDMA(value);
			break;
		case 0x4016:
			nes.getController1().writeByte(value);
			nes.getController2().writeByte(value);
			break;
		default:
			break;
		}
	}
	// Cartridge space (mapper)
	else
	{
		mapper->writeByte(address, value);
	}
}
