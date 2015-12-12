#include "Mapper.hpp"
#include "NES.hpp"
#include "PPU.hpp"

static const uint8_t nametableMirrorLookup[][4] = {
	{0, 0, 1, 1}, // Vertical
	{0, 1, 0, 1}  // Horizontal
};

PPU::PPU(NES& nes) :
	nes(nes)
{
	currentAddress.w = 0;
	writeToggle = false;

	///@todo what is the correct state for these at power-on?
	frame = 0;
	//scanline = 240;
	//cycle = 340;
	scanline = 261;
	cycle = 0;
}

int PPU::getFrame() const
{
	return frame;
}

uint16_t PPU::getNametableIndex( uint16_t address )
{
	address = (address - 0x2000) % 0x1000;
	int table = address / 0x400;
	int offset = address % 0x400;
	int mode = nes.getROMImage().getHeader()->getMirroring();
	return (0x2000 + nametableMirrorLookup[mode][table] * 0x400 + offset) % 2048;
}

uint32_t* PPU::getVisualPatternTable() const
{
	uint32_t* pixels = new uint32_t[128 * 128 * 2];

	int x = 0;
	int y = 0;
	for( int index = 0; index < 0x2000; index += 16 )
	{
		for( int row = 0; row < 8; row++ )
		{
			uint8_t plane1 = nes.getMemory().getMapper().readByte(index + row);
			uint8_t plane2 = nes.getMemory().getMapper().readByte(index + row + 8);

			for( int column = 0; column < 8; column++ )
			{
				uint32_t pixel = 0xff000000 | ((((plane1 & (1 << column)) ? 1 : 0) + ((plane2 & (1 << column)) ? 2 : 0)) * 0x555555);

				pixels[(y + row) * 256 + (x + (7 - column))] = pixel;
			}
		}

		x += 8;
		if( index < 0x1000 )
		{
			if( x >= 128 )
			{
				x = 0;
				y += 8;
				if( y >= 128 )
				{
					x = 128;
					y = 0;
				}
			}
		}
		else
		{
			if( x >= 256 )
			{
				x = 128;
				y += 8;
			}
		}
	}

	return pixels;
}

uint8_t PPU::readByte( uint16_t address )
{
	// Mirror all addresses above $3fff
	address &= 0x3fff;

	if( address < 0x2000 )
	{
		// CHR
		return nes.getMemory().getMapper().readByte(address);
	}
	else if( address < 0x3f00 )
	{
		// Nametable
		return nametable[getNametableIndex(address)];
	}

	return 0;
}

uint8_t PPU::readRegister( uint16_t address )
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
		return (cycle % 2 == 0 ? 0xc0 : 0);
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

void PPU::step()
{
	// Increment the timing counters
	cycle++;
	if( cycle > 340 )
	{
		cycle = 0;
		scanline++;
		if( scanline > 261 )
		{
			scanline = 0;
			frame++;
		}
	}

	// Check for vblank
	if( scanline == 241 && cycle == 1 )
	{
		nes.getCPU().requestNMI();
	}
}

void PPU::writeAddressRegister( uint8_t value )
{
	if( !writeToggle )
	{
		// Upper byte
		currentAddress.h = value;
	}
	else
	{
		// Lower byte
		currentAddress.l = value;
	}
	writeToggle = !writeToggle;
}

void PPU::writeByte( uint16_t address, uint8_t value )
{
	// Mirror all addresses above $3fff
	address &= 0x3fff;

	if( address < 0x2000 )
	{
		// CHR
		nes.getMemory().getMapper().writeByte(address, value);
	}
	else if( address < 0x3f00 )
	{
		// Nametable
		nametable[getNametableIndex(address)] = value;
	}
}

void PPU::writeDataRegister( uint8_t value )
{
	writeByte( currentAddress.w, value );
    if( !registers.PPUCTRL.increment )
	{
		currentAddress.w++;
	}
	else
	{
		currentAddress.w += 32;
	}
}

void PPU::writeRegister( uint16_t address, uint8_t value )
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
		writeAddressRegister(value);
		break;
	// PPUDATA
	case 0x2007:
		writeDataRegister(value);
		break;
	default:
		break;
	}
}
