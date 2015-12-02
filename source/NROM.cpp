#include <iostream>

#include "NES.hpp"
#include "NROM.hpp"

NROM::NROM(NES& nes) :
	nes(nes),
	nrom256(false)
{
	// Check if we are NROM-256 or NROM-128
	if( nes.getROMImage().getHeader()->prgPages == 2 )
	{
		nrom256 = true;
	}
}

void NROM::print() const
{
	std::cout << "************************************************************************\n";
	std::cout << "MAPPER INFORMATION\n";
	std::cout << "Mapper:\t\tNROM\n";
	std::cout << "Variant:\tNROM-" << (nrom256 ? "256" : "128") << std::endl;
	std::cout << "************************************************************************\n";
}

uint8_t NROM::readByte( uint16_t address )
{
	if( address < 0x2000 )
	{
		// CHR
		return nes.getROMImage().getChrPage(0)[address];
	}
	else if( address >= 0x8000 && address <= 0xbfff )
	{
		return (nes.getROMImage().getPrgPage(0))[address - 0x8000];
	}
	else if( address >= 0xc000 )
	{
		// If we are NROM-256, use the upper 16k of ROM, otherwise
		// mirror the lower 16k of ROM
		if( nrom256 )
		{
			return (nes.getROMImage().getPrgPage(1))[address - 0xc000];
		}
		else
		{
			return (nes.getROMImage().getPrgPage(0))[address - 0xc000];
		}
	}

	return 0;
}

void NROM::writeByte( uint16_t address, uint8_t value )
{
	// NROM does not support writing (it only contains ROM)
}
