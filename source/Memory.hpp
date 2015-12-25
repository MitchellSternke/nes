#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "Types.hpp"

class Mapper;
class NES;

/**
 * Wraps all memory access and performs mapping.
 */
class Memory
{
public:
	Memory( NES& nes );

	Mapper& getMapper();
	uint8_t readByte( uint16_t address );
	uint16_t readWord( uint16_t address );
	void writeByte( uint16_t address, uint8_t value );

private:
	NES& nes;
	Mapper* mapper;

	uint8_t ram[0x800]; /**< Internal RAM (2kb). */
};

/**
 * Wraps individual byte memory access.
 */
class MemoryAccess
{
public:
	MemoryAccess( Memory& memory, uint16_t address );

	uint16_t getAddress() const;

	MemoryAccess& operator = ( uint8_t value );
	operator uint8_t();

private:
	Memory& memory;
	uint16_t address;
	uint8_t value;
	bool valueInitialized;
};

#endif // MEMORY_HPP
