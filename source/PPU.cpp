#include <iostream>

#include "Mapper.hpp"
#include "NES.hpp"
#include "PPU.hpp"

static const uint8_t nametableMirrorLookup[][4] = {
	{0, 0, 1, 1}, // Vertical
	{0, 1, 0, 1}  // Horizontal
};

/**
 * RGB representation of the NES palette.
 */
static const uint32_t paletteRGB[] = {
	0x7c7c7c,
	0x0000fc,
	0x0000bc,
	0x4428bc,
	0x940084,
	0xa80020,
	0xa81000,
	0x881400,
	0x503000,
	0x007800,
	0x006800,
	0x005800,
	0x004058,
	0x000000,
	0x000000,
	0x000000,
	0xbcbcbc,
	0x0078f8,
	0x0058f8,
	0x6844fc,
	0xd800cc,
	0xe40058,
	0xf83800,
	0xe45c10,
	0xac7c00,
	0x00b800,
	0x00a800,
	0x00a844,
	0x008888,
	0x000000,
	0x000000,
	0x000000,
	0xf8f8f8,
	0x3cbcfc,
	0x6888fc,
	0x9878f8,
	0xf878f8,
	0xf85898,
	0xf87858,
	0xfca044,
	0xf8b800,
	0xb8f818,
	0x58d854,
	0x58f898,
	0x00e8d8,
	0x787878,
	0x000000,
	0x000000,
	0xfcfcfc,
	0xa4e4fc,
	0xb8b8f8,
	0xd8b8f8,
	0xf8b8f8,
	0xf8a4c0,
	0xf0d0b0,
	0xfce0a8,
	0xf8d878,
	0xd8f878,
	0xb8f8b8,
	0xb8f8d8,
	0x00fcfc,
	0xf8d8f8,
	0x000000,
	0x000000
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

uint8_t PPU::getAttributeTableValue( uint16_t nametableAddress )
{
	nametableAddress = getNametableIndex(nametableAddress);

	// Determine the 32x32 attribute table address
	int row = ((nametableAddress & 0x3e0) >> 5) / 4;
	int col = (nametableAddress & 0x1f) / 4;

	// Determine the 16x16 metatile for the 8x8 tile addressed
	int shift = ((nametableAddress & BIT_6) ? 4 : 0) + ((nametableAddress & BIT_1) ? 2 : 0);

	// Determine the offset into the attribute table
	int offset = (nametableAddress & 0xc00) + 0x400 - 64 + (row * 8 + col);

	// Determine the attribute table value
	return (nametable[offset] & (0x3 << shift)) >> shift;
}

uint16_t PPU::getNametableIndex( uint16_t address )
{
	address = (address - 0x2000) % 0x1000;
	int table = address / 0x400;
	int offset = address % 0x400;
	int mode = nes.getROMImage().getHeader()->getMirroring();
	return (nametableMirrorLookup[mode][table] * 0x400 + offset) % 2048;
}

uint32_t* PPU::getVisualNametable()
{
	uint32_t* pixels = new uint32_t[256 * 2 * 240 * 2];

	int x = 0;
	int y = 0;
	for( int index = 0x2000; index < 0x3000; index++ )
	{
		// Lookup the pattern table entry
		uint16_t tile = readByte(index) + 256;
		uint8_t attribute = getAttributeTableValue(index);

		// Read the pixels of the tile
		for( int row = 0; row < 8; row++ )
		{
			uint8_t plane1 = nes.getMemory().getMapper().readByte(tile * 16 + row);
			uint8_t plane2 = nes.getMemory().getMapper().readByte(tile * 16 + row + 8);

			for( int column = 0; column < 8; column++ )
			{
				//uint32_t pixel = 0xff000000 | ((((plane1 & (1 << column)) ? 1 : 0) + ((plane2 & (1 << column)) ? 2 : 0)) * 0x555555);
				uint8_t paletteIndex = (((plane1 & (1 << column)) ? 1 : 0) + ((plane2 & (1 << column)) ? 2 : 0));
				uint8_t colorIndex = palette[attribute * 4 + paletteIndex];
				if( paletteIndex == 0 )
				{
					colorIndex = palette[0];
				}
				uint32_t pixel = 0xff000000 | paletteRGB[colorIndex];

				pixels[(y + row) * 512 + (x + (7 - column))] = pixel;
			}
		}

		x += 8;
		if( index < 0x2400 )
		{
			if( x >= 256 )
			{
				x = 0;
				y += 8;
				if( y >= 240 )
				{
					x = 256;
					y = 0;
					index = 0x2400 - 1;
				}
			}
		}
		else if( index < 0x2800 )
		{
			if( x >= 512 )
			{
				x = 256;
				y += 8;
				if( y >= 240 )
				{
					x = 0;
					y = 240;
					index = 0x2800 - 1;
				}
			}
		}
		else if( index < 0x2c00 )
		{
			if( x >= 256 )
			{
				x = 0;
				y += 8;
				if( y >= 480 )
				{
					x = 256;
					y = 240;
					index = 0x2c00 - 1;
				}
			}
		}
		else
		{
			if( x >= 512 )
			{
				x = 256;
				y += 8;
				if( y >= 480 )
				{
					break;
				}
			}
		}
	}

	return pixels;
}

uint32_t* PPU::getVisualPalette()
{
	uint32_t* pixels = new uint32_t[8 * 4];

	for( int index = 0; index < 8; index++ )
	{
		pixels[index * 4]     = 0xff000000 | paletteRGB[palette[index * 4]];
		pixels[index * 4 + 1] = 0xff000000 | paletteRGB[palette[index * 4 + 1]];
		pixels[index * 4 + 2] = 0xff000000 | paletteRGB[palette[index * 4 + 2]];
		pixels[index * 4 + 3] = 0xff000000 | paletteRGB[palette[index * 4 + 3]];
	}

	return pixels;
}

uint32_t* PPU::getVisualPatternTable()
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
		writeToggle = false;
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
		if( registers.PPUCTRL.nmiEnable )
		{
			nes.getCPU().requestNMI();
		}
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
	else if( address < 0x3f20 )
	{
		// Palette data
		palette[address - 0x3f00] = value;

		// Mirroring
		if( address == 0x3f10 || address == 0x3f14 || address == 0x3f18 || address == 0x3f1c )
		{
			palette[address - 0x3f10] = value;
		}
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
		registers.PPUCTRL.raw = value;
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
