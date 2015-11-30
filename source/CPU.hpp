#ifndef CPU_HPP
#define CPU_HPP

#include "Types.hpp"

class MemoryAccess;
class NES;

/**
 * Memory addressing modes for instructions.
 */
enum MemoryAddressingMode
{
	MEM_IMMEDIATE,

	MEM_ABSOLUTE,
	MEM_ZERO_PAGE_ABSOLUTE, // a.k.a. zero page
	MEM_INDEXED_X, // a.k.a. absolute, x
	MEM_INDEXED_Y, // a.k.a. absolute, y
	MEM_ZERO_PAGE_INDEXED_X,
	MEM_ZERO_PAGE_INDEXED_Y,
	MEM_INDIRECT,
	MEM_PRE_INDEXED_INDIRECT,
	MEM_POST_INDEXED_INDIRECT,
	MEM_RELATIVE
};

/**
 * CPU registers.
 */
enum Register
{
	REGISTER_A,
	REGISTER_X,
	REGISTER_Y,
	REGISTER_S,
	REGISTER_PC,
	REGISTER_P
};

/**
 * Stores state information for the 6502 2A03 microprocessor used in the NES.
 */
class CPU
{
public:
	CPU( NES& nes );

	/**
	 * Execute the next instruction.
	 */
	void executeNextInstruction();

private:
	//*****************************************************************
	// Types and classes used by the CPU
	//*****************************************************************

	/**
	 * Type used for opcode handler functions.
	 */
	typedef void (CPU::*OpcodeHandler)(void);

	/**
	 * Contains all registers needed for the CPU.
	 */
	struct Registers
	{
		uint8_t a;  /**< The A (Accumulator) register. */
		uint8_t x;  /**< The X (index) register. */
		uint8_t y;  /**< The Y (index) register. */
		uint8_t s;  /**< The stack pointer register. */
		Word    pc; /**< The program counter register. */

		/**
		 * The status/flags register.
		 */
		union
		{
			uint8_t raw;       /**< Raw value of the entire p register. */
			Bit<0>  carry;     /**< Carry. */
			Bit<1>  zero;      /**< Zero. */
			Bit<2>  interrupt; /**< Interrupt enable/disable. */
			Bit<3>  decimal;   /**< Decimal mode. */
			Bit<4>  brk;       /**< Break. */
			Bit<6>  overflow;  /**< Overflow. */
			Bit<7>  sign;      /**< Sign flag. */
		} p;
	};

	/**
	 * Accesses an individual register.
	 */
	template <Register R>
	class RegisterAccess
	{
	public:
		RegisterAccess( Registers& registers );

		RegisterAccess<R>& operator = ( uint8_t value );
		operator uint8_t();

	private:
		Registers& registers;
	};

	//*****************************************************************
	// Member variables
	//*****************************************************************

	NES& nes;
	Registers registers;
	OpcodeHandler opcodes[0x100];

	//*****************************************************************
	// Member functions
	//*****************************************************************

	uint8_t getImmediate8();
	uint16_t getImmediate16();

	template <MemoryAddressingMode M>
	MemoryAccess getMemory();

	template <Register R>
	RegisterAccess<R> getRegister();

	void powerOn();
	void setSign( uint8_t value );
	void setZero( uint8_t value );

	//*****************************************************************
	// Opcode templates
	//*****************************************************************

	/**
	 * ADC opcode template.
	 */
	template <MemoryAddressingMode M>
	void opADC();
};

#endif // CPU_HPP
