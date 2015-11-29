#include <iostream>

#include "ROMImage.hpp"

void ROMHeader::print() const
{
	std::cout << "************************************************************************\n";
	std::cout << "ROM HEADER INFORMATION\n";
	std::cout << "Bytes 0-3:\t\t" << header << "\n";
	std::cout << "Program ROM Pages:\t" << (uint16_t)prgPages << "\n";
	std::cout << "Program CHR Pages:\t" << (uint16_t)chrPages << "\n";
	std::cout << "************************************************************************\n";
}

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
