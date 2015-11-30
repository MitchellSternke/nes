#ifndef ROMIMAGE_HPP
#define ROMIMAGE_HPP

#include "Types.hpp"

/**
 * Represents the iNES header present in most roms.
 */
struct ROMHeader
{
	uint8_t header[4];   /**< Should be "NES<EOF>" in all ROMs. */
	uint8_t prgPages; /**< Number of 16k program ROM pages. */
	uint8_t chrPages;  /**< Number of 8k character ROM pages. */
	uint8_t flags6;
	uint8_t flags7;
	uint8_t unused[8];

	/**
	 * Get the mapper number used by the ROM.
	 */
	uint8_t getMapper() const;

	/**
	 * Print the header in human-readable form.
	 */
	void print() const;
};

/**
 * Represents a ROM image of a game cart.
 */
class ROMImage
{
public:
	/**
	 * Create a ROM image from raw file data.
	 */
	ROMImage( uint8_t* data );

	const uint8_t* getData() const;
	const ROMHeader* getHeader() const;
	const uint8_t* getPrgPage( int index ) const;
	const uint8_t* getChrPage( int index ) const;

private:
	uint8_t* data;
};


#endif // ROMIMAGE_HPP
