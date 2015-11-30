#include "PPU.hpp"

PPU::PPU(NES& nes) :
	nes(nes)
{
}

uint8_t PPU::readByte( uint16_t address )
{
	switch( address )
	{
	// PPUCTRL
	case 0x2000:
		break;
	// PPUMASK
	case 0x2001:
		break;
	// PPUSTATUS
	case 0x2002:
		return 0x80;
		break;
	// OAMADDR
	case 0x2003:
		break;
	// OAMDATA
	case 0x2004:
		break;
	// PPUSCROLL
	case 0x2005:
		break;
	// PPUADDR
	case 0x2006:
		break;
	// PPUDATA
	case 0x2007:
		break;
	default:
		break;
	}

	return 0;
}

void PPU::writeByte( uint16_t address, uint8_t value )
{
	switch( address )
	{
	// PPUCTRL
	case 0x2000:
		break;
	// PPUMASK
	case 0x2001:
		break;
	// PPUSTATUS
	case 0x2002:
		break;
	// OAMADDR
	case 0x2003:
		break;
	// OAMDATA
	case 0x2004:
		break;
	// PPUSCROLL
	case 0x2005:
		break;
	// PPUADDR
	case 0x2006:
		break;
	// PPUDATA
	case 0x2007:
		break;
	default:
		break;
	}
}