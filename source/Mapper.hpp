#ifndef MAPPER_HPP
#define MAPPER_HPP

#include "Types.hpp"

/**
 * Interface for memory mappers.
 */
class Mapper
{
public:
	/**
	 * Print information about the mapper.
	 */
	virtual void print() const =0;

	/**
	 * Read a byte from the mapper.
	 */
	virtual uint8_t readByte( uint16_t address )=0;

	/**
	 * Write a byte to the mapper.
	 */
	virtual void writeByte( uint16_t address, uint8_t value )=0;
};

#endif // MAPPER_HPP
