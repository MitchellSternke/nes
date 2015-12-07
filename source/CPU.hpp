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
	MEM_PRE_INDEXED_INDIRECT, // a.k.a (indirect, x)
	MEM_POST_INDEXED_INDIRECT, // a.k.a. (indirect), y
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
	 * Request a Non-Maskable Interrupt (NMI) on the next instruction.
	 */
	void requestNMI();

	/**
	 * Step CPU emulation by one instruction.
	 *
	 * @return the number of cycles taken to execute the instruction.
	 */
	int step();

private:
	//*****************************************************************
	// Types and classes used by the CPU
	//*****************************************************************

	/**
	 * Type used for opcode handler functions.
	 */
	typedef void (CPU::*OpcodeHandler)(void);

	/**
	 * Interrupts handled by the CPU.
	 */
	enum Interrupt
	{
		INTERRUPT_NONE,

		INTERRUPT_NMI
	};

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
	Interrupt interrupt;
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

	/**
	 * Pull a value from the top of the stack.
	 */
	uint8_t pull();

	/**
	 * Push a value to the top of the stack.
	 */
	void push( uint8_t value );

	void setSign( uint8_t value );
	void setZero( uint8_t value );

	//*****************************************************************
	// Opcode templates and methods
	//*****************************************************************

	/**
	 * ADC opcode template.
	 */
	template <MemoryAddressingMode M>
	void opADC();

	/**
	 * AND opcode template.
	 */
	template <MemoryAddressingMode M>
	void opAND();

	/**
	 * BCC opcode.
	 */
	void opBCC();

	/**
	 * BCS opcode.
	 */
	void opBCS();

	/**
	 * BEQ opcode.
	 */
	void opBEQ();

	/**
	 * BIT opcode template.
	 */
	template <MemoryAddressingMode M>
	void opBIT();

	/**
	 * BNE opcode.
	 */
	void opBNE();

	/**
	 * BPL opcode.
	 */
	void opBPL();

	/**
	 * CLD opcode.
	 */
	void opCLD();

	/**
	 * CMP opcode template.
	 */
	template <MemoryAddressingMode M>
	void opCMP();

	/**
	 * CPX opcode template.
	 */
	template <MemoryAddressingMode M>
	void opCPX();

	/**
	 * CPY opcode template.
	 */
	template <MemoryAddressingMode M>
	void opCPY();

	/**
	 * DEC opcode template.
	 */
	template <MemoryAddressingMode M>
	void opDEC();

	/**
	 * DEX opcode.
	 */
	void opDEX();

	/**
	 * DEY opcode.
	 */
	void opDEY();

	/**
	 * INC opcode template.
	 */
	template <MemoryAddressingMode M>
	void opINC();

	/**
	 * INX opcode.
	 */
	void opINX();

	/**
	 * INY opcode.
	 */
	void opINY();

	/**
	 * JMP opcode template.
	 */
	template <MemoryAddressingMode M>
	void opJMP();

	/**
	 * JSR opcode.
	 */
	void opJSR();

	/**
	 * LDA opcode template.
	 */
	template <MemoryAddressingMode M>
	void opLDA();

	/**
	 * LDX opcode template.
	 */
	template <MemoryAddressingMode M>
	void opLDX();

	/**
	 * LDY opcode template.
	 */
	template <MemoryAddressingMode M>
	void opLDY();

	/**
	 * LSR opcode template.
	 */
	template <MemoryAddressingMode M>
	void opLSR();

	/**
	 * LSR with the accumulator opcode.
	 */
	void opLSRAccumulator();

	/**
	 * ORA opcode template.
	 */
	template <MemoryAddressingMode M>
	void opORA();

	/**
	 * PHA opcode.
	 */
	void opPHA();

	/**
	 * PHP opcode.
	 */
	void opPHP();

	/**
	 * PLA opcode.
	 */
	void opPLA();

	/**
	 * PLP opcode.
	 */
	void opPLP();

	/**
	 * ROL opcode template.
	 */
	template <MemoryAddressingMode M>
	void opROL();

	/**
	 * ROL with the accumulator opcode.
	 */
	void opROLAccumulator();

	/**
	 * ROR opcode template.
	 */
	template <MemoryAddressingMode M>
	void opROR();

	/**
	 * ROR with the accumulator opcode.
	 */
	void opRORAccumulator();

	/**
	 * RTS opcode.
	 */
	void opRTS();

	/**
	 * SBC opcode template.
	 */
	template <MemoryAddressingMode M>
	void opSBC();

	/**
	 * SEC opcode.
	 */
	void opSEC();

	/**
	 * SEI opcode.
	 */
	void opSEI();

	/**
	 * STA opcode template.
	 */
	template <MemoryAddressingMode M>
	void opSTA();

	/**
	 * STX opcode template.
	 */
	template <MemoryAddressingMode M>
	void opSTX();

	/**
	 * STY opcode template.
	 */
	template <MemoryAddressingMode M>
	void opSTY();

	/**
	 * TAX opcode.
	 */
	void opTAX();

	/**
	 * TAY opcode.
	 */
	void opTAY();

	/**
	 * TSX opcode.
	 */
	void opTSX();

	/**
	 * TXA opcode.
	 */
	void opTXA();

	/**
	 * TXS opcode.
	 */
	void opTXS();

	/**
	 * TYA opcode.
	 */
	void opTYA();
};

#endif // CPU_HPP
