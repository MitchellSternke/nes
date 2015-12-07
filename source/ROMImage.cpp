#include <iostream>

#include "ROMImage.hpp"

//*********************************************************************
// ROMHeader struct
//*********************************************************************

uint8_t ROMHeader::getMapper() const
{
	return ((flags6 & 0xf0) >> 4) | (flags7 & 0xf0);
}

uint8_t ROMHeader::getMirroring() const
{
	return ((flags6 & BIT_3) >> 2) | (flags6 & BIT_0);
}

void ROMHeader::print() const
{
	std::cout << "************************************************************************\n";
	std::cout << "ROM HEADER INFORMATION\n";
	std::cout << "Bytes 0-3:\t\t" << header << "\n";
	std::cout << "Program ROM Pages:\t" << (int)prgPages << "\n";
	std::cout << "Program CHR Pages:\t" << (int)chrPages << "\n";
	std::cout << "Mapper Number:\t\t" << (int)getMapper() << "\n";
	std::cout << "Mirroring Mode:\t\t" << (int)getMirroring() << "\n";
	std::cout << "************************************************************************\n";
}

//*********************************************************************
// ROMImage class
//*********************************************************************

ROMImage::ROMImage( uint8_t* data )
{
	this->data = data;
}

const uint8_t* ROMImage::getData() const
{
	return (data + sizeof(ROMHeader));
}

const ROMHeader* ROMImage::getHeader() const
{
	return (ROMHeader*)(data);
}

const uint8_t* ROMImage::getPrgPage( int index ) const
{
	return (data + 16 + (16384 * index));
}

const uint8_t* ROMImage::getChrPage( int index ) const
{
	return (data + 16 + (16384 * getHeader()->prgPages) + (8192 * index));
}
