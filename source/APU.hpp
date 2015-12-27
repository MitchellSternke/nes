#ifndef APU_HPP
#define APU_HPP

#include "Types.hpp"

/**
 * Emulates the Audio Procesing Unit (APU).
 */
class APU
{
public:
	/**
	 * Read a byte from one of the APU's registers.
	 */
	uint8_t readByte( uint16_t address );

	/**
	 * Write a byte to one of the APU's registers.
	 */
	void writeByte( uint16_t address, uint8_t value );
};

#endif // APU_HPP
