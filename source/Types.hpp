#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>

/**
 * A 16-bit word.
 */
union Word
{
	struct
	{
		uint8_t l; /**< low byte. */
		uint8_t h; /**< high byte. */
	};
	uint16_t w; /**< full word */
};

/**
 * Provides access to selected bits in a bit field.
 */
template <unsigned bit, unsigned bitCount = 1, typename T = uint8_t>
struct Bit
{
	T data;
	static constexpr T mask = (1u << bitCount) - 1u;

	template <typename U>
	Bit& operator = ( U value )
	{
		data = (data & ~(mask << bit)) | ((bitCount > 1 ? value & mask : !!value) << bit);
		return *this;
	}

	operator unsigned() const
	{
		return (data >> bit) & mask;
	}

	Bit& operator ++ ()
	{
		return *this = *this + 1;
	}

	unsigned operator ++ (int)
	{
		unsigned r = *this;
		++*this;
		return r;
	}
};

#define BIT_0  (1 << 0)
#define BIT_1  (1 << 1)
#define BIT_2  (1 << 2)
#define BIT_3  (1 << 3)
#define BIT_4  (1 << 4)
#define BIT_5  (1 << 5)
#define BIT_6  (1 << 6)
#define BIT_7  (1 << 7)

#endif // TYPES_HPP
