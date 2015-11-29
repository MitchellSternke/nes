#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "Types.hpp"

class NES;

/**
 * Wraps all memory access and performs mapping.
 */
class Memory
{
public:
	Memory( NES& nes );

	uint8_t readByte( uint16_t address );
	uint16_t readWord( uint16_t address );
	void writeByte( uint16_t address, uint8_t value );

private:
	NES& nes;
};

/**
 * Wraps individual byte memory access.
 */
class MemoryAccess
{
public:
	MemoryAccess( Memory& memory, uint16_t address );

	MemoryAccess& operator = ( uint8_t value );
	operator uint8_t();

private:
	Memory& memory;
	uint16_t address;
};

#endif // MEMORY_HPP
