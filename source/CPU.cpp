#include <cstdlib>
#include <iostream>

#include <boost/format.hpp>

#include "CPU.hpp"
#include "NES.hpp"

// Addresses for interrupt vectors
#define VECTOR_NMI   0xfffa
#define VECTOR_RESET 0xfffc
#define VECTOR_IRQ   0xfffe

//*********************************************************************
// The Register template
//*********************************************************************

template <Register R>
CPU::RegisterAccess<R>::RegisterAccess( CPU::Registers& registers ) :
	registers(registers)
{
}

template <Register R>
CPU::RegisterAccess<R>& CPU::RegisterAccess<R>::operator = ( uint8_t value )
{
	switch(R)
	{
	case REGISTER_A:
		registers.a = value;
		break;
	case REGISTER_X:
		registers.x = value;
		break;
	case REGISTER_Y:
		registers.y = value;
		break;
	case REGISTER_S:
		registers.s = value;
		break;
	case REGISTER_PC:
		registers.pc.l = value;
		break;
	case REGISTER_P:
		registers.p.raw = value;
		break;
	}

	return (*this);
}

template <Register R>
CPU::RegisterAccess<R>::operator uint8_t()
{
	switch(R)
	{
	case REGISTER_A:
		return registers.a;
	case REGISTER_X:
		return registers.x;
	case REGISTER_Y:
		return registers.y;
	case REGISTER_S:
		return registers.s;
	case REGISTER_PC:
		return registers.pc.l;
	case REGISTER_P:
		return registers.p.raw;
	}
}

//*********************************************************************
// The CPU class
//*********************************************************************

CPU::CPU( NES& nes ) :
	nes(nes)
{
	// Reset to the initial power on state
	powerOn();

	// Initialize the opcode table
	for( int i = 0; i < 0x100; i++ )
	{
		opcodes[i] = nullptr;
	}

	// ADC
	opcodes[0x69] = &CPU::opADC<MEM_IMMEDIATE>;
	opcodes[0x65] = &CPU::opADC<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0x75] = &CPU::opADC<MEM_ZERO_PAGE_INDEXED_X>;
	opcodes[0x60] = &CPU::opADC<MEM_ABSOLUTE>;
	opcodes[0x70] = &CPU::opADC<MEM_INDEXED_X>;
	opcodes[0x79] = &CPU::opADC<MEM_INDEXED_Y>;
	opcodes[0x61] = &CPU::opADC<MEM_PRE_INDEXED_INDIRECT>;
	opcodes[0x71] = &CPU::opADC<MEM_POST_INDEXED_INDIRECT>;

	// CLD
	opcodes[0xd8] = &CPU::opCLD;

	// LDA
	opcodes[0xa9] = &CPU::opLDA<MEM_IMMEDIATE>;
	opcodes[0xa5] = &CPU::opLDA<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0xb5] = &CPU::opLDA<MEM_ZERO_PAGE_INDEXED_X>;
	opcodes[0xad] = &CPU::opLDA<MEM_ABSOLUTE>;
	opcodes[0xbd] = &CPU::opLDA<MEM_INDEXED_X>;
	opcodes[0xb9] = &CPU::opLDA<MEM_INDEXED_Y>;
	opcodes[0xa1] = &CPU::opLDA<MEM_PRE_INDEXED_INDIRECT>;
	opcodes[0xb1] = &CPU::opLDA<MEM_POST_INDEXED_INDIRECT>;

	// LDX
	opcodes[0xa2] = &CPU::opLDX<MEM_IMMEDIATE>;
	opcodes[0xa6] = &CPU::opLDX<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0xb6] = &CPU::opLDX<MEM_ZERO_PAGE_INDEXED_Y>;
	opcodes[0xae] = &CPU::opLDX<MEM_ABSOLUTE>;
	opcodes[0xbe] = &CPU::opLDX<MEM_INDEXED_Y>;

	// SEI
	opcodes[0x78] = &CPU::opSEI;

	// STA
	opcodes[0x85] = &CPU::opSTA<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0x95] = &CPU::opSTA<MEM_ZERO_PAGE_INDEXED_X>;
	opcodes[0x8d] = &CPU::opSTA<MEM_ABSOLUTE>;
	opcodes[0x9d] = &CPU::opSTA<MEM_INDEXED_X>;
	opcodes[0x99] = &CPU::opSTA<MEM_INDEXED_Y>;
	opcodes[0x81] = &CPU::opSTA<MEM_PRE_INDEXED_INDIRECT>;
	opcodes[0x91] = &CPU::opSTA<MEM_POST_INDEXED_INDIRECT>;

	// TXS
	opcodes[0x9a] = &CPU::opTXS;
}

void CPU::executeNextInstruction()
{
	// Fetch the opcode
	uint8_t opcode = nes.getMemory().readByte(registers.pc.w);
	if( opcodes[opcode] == nullptr )
	{
		std::cout << boost::format("Error: unimplemented opcode: %02X") % (uint16_t)opcode << std::endl;
		exit(-1);
	}

	// Execute the opcode
	registers.pc.w++;
	((*this).*(opcodes[opcode]))();
}

uint8_t CPU::getImmediate8()
{
	uint8_t value = nes.getMemory().readByte(registers.pc.w);
	registers.pc.w++;

	return value;
}

uint16_t CPU::getImmediate16()
{
	uint16_t value = nes.getMemory().readWord(registers.pc.w);
	registers.pc.w += 2;

	return value;
}

template <MemoryAddressingMode M>
MemoryAccess CPU::getMemory()
{
	switch(M)
	{
	case MEM_IMMEDIATE:
		{
			uint16_t pc = registers.pc.w;
			registers.pc.w++;
			return MemoryAccess(nes.getMemory(), pc);
		}
	case MEM_ABSOLUTE:
		return MemoryAccess(nes.getMemory(), getImmediate16());
	case MEM_ZERO_PAGE_ABSOLUTE:
		return MemoryAccess(nes.getMemory(), getImmediate8());
	case MEM_INDEXED_X:
		return MemoryAccess(nes.getMemory(), getImmediate16() + registers.x);
	case MEM_INDEXED_Y:
		return MemoryAccess(nes.getMemory(), getImmediate16() + registers.y);
	case MEM_ZERO_PAGE_INDEXED_X:
		return MemoryAccess(nes.getMemory(), getImmediate8() + registers.x);
	case MEM_ZERO_PAGE_INDEXED_Y:
		return MemoryAccess(nes.getMemory(), getImmediate8() + registers.y);
	case MEM_INDIRECT:
		return MemoryAccess(nes.getMemory(), nes.getMemory().readWord(getImmediate16()));
	case MEM_PRE_INDEXED_INDIRECT:
		return MemoryAccess(nes.getMemory(), nes.getMemory().readWord(getImmediate8() + registers.x));
	case MEM_POST_INDEXED_INDIRECT:
		return MemoryAccess(nes.getMemory(), nes.getMemory().readWord(getImmediate8()) + registers.y);
	case MEM_RELATIVE:
		{
			uint16_t address;
			uint16_t offset = getImmediate8();
			if( offset < 0x80 )
			{
				address = getImmediate8() + offset;
			}
			else
			{
				address = getImmediate8() + offset - 0x100;
			}
			return MemoryAccess(nes.getMemory(), address);
		}
	}
}

void CPU::powerOn()
{
	// Reset the CPU to power on state
	registers.p.raw = 0x34;
	registers.a = 0;
	registers.x = 0;
	registers.y = 0;
	registers.s = 0xfd;

	// Jump to the reset vector for the first instruction
	registers.pc.w = nes.getMemory().readWord(VECTOR_RESET);
}

void CPU::setSign( uint8_t value )
{
	registers.p.sign = ((value & BIT_7) ? 1 : 0);
}

void CPU::setZero( uint8_t value )
{
	registers.p.zero = ((value == 0) ? 1 : 0);
}

//*********************************************************************
// Opcode templates and methods
//*********************************************************************

template <MemoryAddressingMode M>
void CPU::opADC()
{
	MemoryAccess src = getMemory<M>();
	uint16_t temp = src + registers.a + (registers.p.carry ? 1 : 0);
	setZero(temp & 0xff);
	if( registers.p.decimal )
	{
		if( ((registers.a & 0xf) + (src & 0xf) + (registers.p.carry ? 1 : 0)) > 9 )
		{
			temp += 6;
		}
		setSign(temp);
		registers.p.overflow = (!((registers.a ^ src) & 0x80) && ((registers.a ^ temp) & 0x80));
		if( temp > 0x99 )
		{
			temp += 96;
		}
		registers.p.carry = (temp > 0x99);
	}
	else
	{
		setSign(temp);
		registers.p.overflow = (!((registers.a ^ src) & 0x80) && ((registers.a ^ temp) & 0x80));
		registers.p.carry = (temp > 0xff);
	}
	registers.a = (uint8_t)temp;
}

void CPU::opCLD()
{
	registers.p.decimal = 0;
}

template <MemoryAddressingMode M>
void CPU::opLDA()
{
	MemoryAccess src = getMemory<M>();
	setSign(src);
	setZero(src);
	registers.a = src;
}

template <MemoryAddressingMode M>
void CPU::opLDX()
{
	MemoryAccess src = getMemory<M>();
	setSign(src);
	setZero(src);
	registers.x = src;
}

void CPU::opSEI()
{
	registers.p.interrupt = 1;
}

template <MemoryAddressingMode M>
void CPU::opSTA()
{
	MemoryAccess dest = getMemory<M>();
	dest = registers.a;
}

void CPU::opTXS()
{
	registers.s = registers.x;
}
