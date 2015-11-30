#ifndef NROM_HPP
#define NROM_HPP

#include "Mapper.hpp"

class NES;

/**
 * iNES mapper 0: NROM.
 */
class NROM : public Mapper
{
public:
	NROM(NES& nes);

	void print() const;
	uint8_t readByte( uint16_t address );
	void writeByte( uint16_t address, uint8_t value );

private:
	NES& nes;
	bool nrom256;
};

#endif // NROM_HPP
