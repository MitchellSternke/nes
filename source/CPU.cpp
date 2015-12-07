#include <cstdlib>
#include <iostream>

#include <boost/format.hpp>

#include "CPU.hpp"
#include "NES.hpp"

// Addresses for interrupt vectors
#define VECTOR_NMI   0xfffa
#define VECTOR_RESET 0xfffc
#define VECTOR_IRQ   0xfffe

// Minimum number of CPU cycles needed to execute each Opcode
static const uint8_t instructionCycles[] = {
	7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
	2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
	2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
};

// Instruction Names for each Opcode
static const char* instructionNames[] = {
	"BRK", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
	"PHP", "ORA", "ASL", "ANC", "NOP", "ORA", "ASL", "SLO",
	"BPL", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
	"CLC", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO",
	"JSR", "AND", "KIL", "RLA", "BIT", "AND", "ROL", "RLA",
	"PLP", "AND", "ROL", "ANC", "BIT", "AND", "ROL", "RLA",
	"BMI", "AND", "KIL", "RLA", "NOP", "AND", "ROL", "RLA",
	"SEC", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA",
	"RTI", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
	"PHA", "EOR", "LSR", "ALR", "JMP", "EOR", "LSR", "SRE",
	"BVC", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
	"CLI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE",
	"RTS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
	"PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA",
	"BVS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
	"SEI", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA",
	"NOP", "STA", "NOP", "SAX", "STY", "STA", "STX", "SAX",
	"DEY", "NOP", "TXA", "XAA", "STY", "STA", "STX", "SAX",
	"BCC", "STA", "KIL", "AHX", "STY", "STA", "STX", "SAX",
	"TYA", "STA", "TXS", "TAS", "SHY", "STA", "SHX", "AHX",
	"LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX",
	"TAY", "LDA", "TAX", "LAX", "LDY", "LDA", "LDX", "LAX",
	"BCS", "LDA", "KIL", "LAX", "LDY", "LDA", "LDX", "LAX",
	"CLV", "LDA", "TSX", "LAS", "LDY", "LDA", "LDX", "LAX",
	"CPY", "CMP", "NOP", "DCP", "CPY", "CMP", "DEC", "DCP",
	"INY", "CMP", "DEX", "AXS", "CPY", "CMP", "DEC", "DCP",
	"BNE", "CMP", "KIL", "DCP", "NOP", "CMP", "DEC", "DCP",
	"CLD", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP",
	"CPX", "SBC", "NOP", "ISC", "CPX", "SBC", "INC", "ISC",
	"INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISC",
	"BEQ", "SBC", "KIL", "ISC", "NOP", "SBC", "INC", "ISC",
	"SED", "SBC", "NOP", "ISC", "NOP", "SBC", "INC", "ISC",
};

//*********************************************************************
// The RegisterAccess template
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
	opcodes[0x6D] = &CPU::opADC<MEM_ABSOLUTE>;
	opcodes[0x7D] = &CPU::opADC<MEM_INDEXED_X>;
	opcodes[0x79] = &CPU::opADC<MEM_INDEXED_Y>;
	opcodes[0x61] = &CPU::opADC<MEM_PRE_INDEXED_INDIRECT>;
	opcodes[0x71] = &CPU::opADC<MEM_POST_INDEXED_INDIRECT>;

	// AND
	opcodes[0x29] = &CPU::opAND<MEM_IMMEDIATE>;
	opcodes[0x25] = &CPU::opAND<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0x35] = &CPU::opAND<MEM_ZERO_PAGE_INDEXED_X>;
	opcodes[0x2d] = &CPU::opAND<MEM_ABSOLUTE>;
	opcodes[0x3d] = &CPU::opAND<MEM_INDEXED_X>;
	opcodes[0x39] = &CPU::opAND<MEM_INDEXED_Y>;
	opcodes[0x21] = &CPU::opAND<MEM_PRE_INDEXED_INDIRECT>;
	opcodes[0x31] = &CPU::opAND<MEM_POST_INDEXED_INDIRECT>;

	// ASL
	opcodes[0x0a] = &CPU::opASLAccumulator;
	opcodes[0x06] = &CPU::opASL<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0x16] = &CPU::opASL<MEM_ZERO_PAGE_INDEXED_X>;
	opcodes[0x0e] = &CPU::opASL<MEM_ABSOLUTE>;
	opcodes[0x1e] = &CPU::opASL<MEM_INDEXED_X>;

	// BCC
	opcodes[0x90] = &CPU::opBCC;

	// BCS
	opcodes[0xb0] = &CPU::opBCS;

	// BEQ
	opcodes[0xf0] = &CPU::opBEQ;

	// BIT
	opcodes[0x24] = &CPU::opBIT<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0x2c] = &CPU::opBIT<MEM_ABSOLUTE>;

	// BNE
	opcodes[0xd0] = &CPU::opBNE;

	// BPL
	opcodes[0x10] = &CPU::opBPL;

	// CLC
	opcodes[0x18] = &CPU::opCLC;

	// CLD
	opcodes[0xd8] = &CPU::opCLD;

	// CMP
	opcodes[0xc9] = &CPU::opCMP<MEM_IMMEDIATE>;
	opcodes[0xc5] = &CPU::opCMP<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0xd5] = &CPU::opCMP<MEM_ZERO_PAGE_INDEXED_X>;
	opcodes[0xcd] = &CPU::opCMP<MEM_ABSOLUTE>;
	opcodes[0xdd] = &CPU::opCMP<MEM_INDEXED_X>;
	opcodes[0xd9] = &CPU::opCMP<MEM_INDEXED_Y>;
	opcodes[0xc1] = &CPU::opCMP<MEM_PRE_INDEXED_INDIRECT>;
	opcodes[0xd1] = &CPU::opCMP<MEM_POST_INDEXED_INDIRECT>;

	// CPX
	opcodes[0xe0] = &CPU::opCPX<MEM_IMMEDIATE>;
	opcodes[0xe4] = &CPU::opCPX<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0xec] = &CPU::opCPX<MEM_ABSOLUTE>;

	// CPY
	opcodes[0xc0] = &CPU::opCPY<MEM_IMMEDIATE>;
	opcodes[0xc4] = &CPU::opCPY<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0xcc] = &CPU::opCPY<MEM_ABSOLUTE>;

	// DEC
	opcodes[0xc6] = &CPU::opDEC<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0xd6] = &CPU::opDEC<MEM_ZERO_PAGE_INDEXED_X>;
	opcodes[0xce] = &CPU::opDEC<MEM_ABSOLUTE>;
	opcodes[0xde] = &CPU::opDEC<MEM_INDEXED_X>;

	// DEX
	opcodes[0xca] = &CPU::opDEX;

	// DEY
	opcodes[0x88] = &CPU::opDEY;

	// EOR
	opcodes[0x49] = &CPU::opEOR<MEM_IMMEDIATE>;
	opcodes[0x45] = &CPU::opEOR<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0x55] = &CPU::opEOR<MEM_ZERO_PAGE_INDEXED_X>;
	opcodes[0x4d] = &CPU::opEOR<MEM_ABSOLUTE>;
	opcodes[0x5d] = &CPU::opEOR<MEM_INDEXED_X>;
	opcodes[0x59] = &CPU::opEOR<MEM_INDEXED_Y>;
	opcodes[0x41] = &CPU::opEOR<MEM_PRE_INDEXED_INDIRECT>;
	opcodes[0x51] = &CPU::opEOR<MEM_POST_INDEXED_INDIRECT>;

	// INC
	opcodes[0xe6] = &CPU::opINC<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0xf6] = &CPU::opINC<MEM_ZERO_PAGE_INDEXED_X>;
	opcodes[0xee] = &CPU::opINC<MEM_ABSOLUTE>;
	opcodes[0xfe] = &CPU::opINC<MEM_INDEXED_X>;

	// INX
	opcodes[0xe8] = &CPU::opINX;

	// INY
	opcodes[0xc8] = &CPU::opINY;

	// JMP
	opcodes[0x4c] = &CPU::opJMP<MEM_ABSOLUTE>;
	opcodes[0x6c] = &CPU::opJMP<MEM_INDIRECT>;

	// JSR
	opcodes[0x20] = &CPU::opJSR;

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

	// LDY
	opcodes[0xa0] = &CPU::opLDY<MEM_IMMEDIATE>;
	opcodes[0xa4] = &CPU::opLDY<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0xb4] = &CPU::opLDY<MEM_ZERO_PAGE_INDEXED_X>;
	opcodes[0xac] = &CPU::opLDY<MEM_ABSOLUTE>;
	opcodes[0xbc] = &CPU::opLDY<MEM_INDEXED_X>;

	// LSR
	opcodes[0x4a] = &CPU::opLSRAccumulator;
	opcodes[0x46] = &CPU::opLSR<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0x56] = &CPU::opLSR<MEM_ZERO_PAGE_INDEXED_X>;
	opcodes[0x4e] = &CPU::opLSR<MEM_ABSOLUTE>;
	opcodes[0x5e] = &CPU::opLSR<MEM_INDEXED_X>;

	// ORA
	opcodes[0x09] = &CPU::opORA<MEM_IMMEDIATE>;
	opcodes[0x05] = &CPU::opORA<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0x15] = &CPU::opORA<MEM_ZERO_PAGE_INDEXED_X>;
	opcodes[0x0d] = &CPU::opORA<MEM_ABSOLUTE>;
	opcodes[0x1d] = &CPU::opORA<MEM_INDEXED_X>;
	opcodes[0x19] = &CPU::opORA<MEM_INDEXED_Y>;
	opcodes[0x01] = &CPU::opORA<MEM_PRE_INDEXED_INDIRECT>;
	opcodes[0x11] = &CPU::opORA<MEM_POST_INDEXED_INDIRECT>;

	// PHA
	opcodes[0x48] = &CPU::opPHA;

	// PHP
	opcodes[0x08] = &CPU::opPHP;

	// PLA
	opcodes[0x68] = &CPU::opPLA;

	// PLP
	opcodes[0x28] = &CPU::opPLP;

	// ROL
	opcodes[0x2a] = &CPU::opROLAccumulator;
	opcodes[0x26] = &CPU::opROL<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0x36] = &CPU::opROL<MEM_ZERO_PAGE_INDEXED_X>;
	opcodes[0x2e] = &CPU::opROL<MEM_ABSOLUTE>;
	opcodes[0x3e] = &CPU::opROL<MEM_INDEXED_X>;

	// ROR
	opcodes[0x6a] = &CPU::opRORAccumulator;
	opcodes[0x66] = &CPU::opROR<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0x76] = &CPU::opROR<MEM_ZERO_PAGE_INDEXED_X>;
	opcodes[0x6e] = &CPU::opROR<MEM_ABSOLUTE>;
	opcodes[0x7e] = &CPU::opROR<MEM_INDEXED_X>;

	// RTS
	opcodes[0x60] = &CPU::opRTS;

	// SBC
	opcodes[0xe9] = &CPU::opSBC<MEM_IMMEDIATE>;
	opcodes[0xe5] = &CPU::opSBC<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0xf5] = &CPU::opSBC<MEM_ZERO_PAGE_INDEXED_X>;
	opcodes[0xed] = &CPU::opSBC<MEM_ABSOLUTE>;
	opcodes[0xfd] = &CPU::opSBC<MEM_INDEXED_X>;
	opcodes[0xf9] = &CPU::opSBC<MEM_INDEXED_Y>;
	opcodes[0xe1] = &CPU::opSBC<MEM_PRE_INDEXED_INDIRECT>;
	opcodes[0xf1] = &CPU::opSBC<MEM_POST_INDEXED_INDIRECT>;

	// SEC
	opcodes[0x38] = &CPU::opSEC;

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

	// STX
	opcodes[0x86] = &CPU::opSTX<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0x96] = &CPU::opSTX<MEM_ZERO_PAGE_INDEXED_Y>;
	opcodes[0x8e] = &CPU::opSTX<MEM_ABSOLUTE>;

	// STY
	opcodes[0x84] = &CPU::opSTY<MEM_ZERO_PAGE_ABSOLUTE>;
	opcodes[0x94] = &CPU::opSTY<MEM_ZERO_PAGE_INDEXED_X>;
	opcodes[0x8c] = &CPU::opSTY<MEM_ABSOLUTE>;

	// TAX
	opcodes[0xaa] = &CPU::opTAX;

	// TAY
	opcodes[0xa8] = &CPU::opTAY;

	// TSX
	opcodes[0xba] = &CPU::opTSX;

	// TXA
	opcodes[0x8a] = &CPU::opTXA;

	// TXS
	opcodes[0x9a] = &CPU::opTXS;

	// TYA
	opcodes[0x98] = &CPU::opTYA;
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

	interrupt = INTERRUPT_NONE;

	// Jump to the reset vector for the first instruction
	registers.pc.w = nes.getMemory().readWord(VECTOR_RESET);
}

uint8_t CPU::pull()
{
	registers.s++;
	return nes.getMemory().readByte(0x100 | (uint16_t)registers.s);
}

void CPU::push( uint8_t value )
{
	nes.getMemory().writeByte(0x100 | (uint16_t)registers.s, value);
	registers.s--;
}

void CPU::requestNMI()
{
	interrupt = INTERRUPT_NMI;
}

void CPU::setSign( uint8_t value )
{
	registers.p.sign = ((value & BIT_7) ? 1 : 0);
}

void CPU::setZero( uint8_t value )
{
	registers.p.zero = ((value == 0) ? 1 : 0);
}

int CPU::step()
{
	int cycles = 0;

	// Check for Interrupts
	switch( interrupt )
	{
	case INTERRUPT_NMI:
		std::cout << "HANDLING NMI\n";
		push(registers.pc.h);
		push(registers.pc.l);
		opPHP();
		registers.pc.w = nes.getMemory().readWord(VECTOR_NMI);
		registers.p.interrupt = 1;
		cycles += 7;
	default:
		break;
	}
	interrupt = INTERRUPT_NONE;

	// Fetch the opcode
	uint8_t opcode = nes.getMemory().readByte(registers.pc.w);
	if( opcodes[opcode] == nullptr )
	{
		std::cout << boost::format("Error: unimplemented opcode: %02X") % (uint16_t)opcode << std::endl;
		exit(-1);
	}

	std::cout << boost::format("%04X: %s %02X %02X\n") % registers.pc.w % instructionNames[opcode] % (uint16_t)nes.getMemory().readByte(registers.pc.w + 1) % (uint16_t)nes.getMemory().readByte(registers.pc.w + 2);

	// Execute the instruction
	registers.pc.w++;
	((*this).*(opcodes[opcode]))();

	///@todo more accurate cycle counting
	cycles += instructionCycles[opcode];
	return cycles;
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

template <MemoryAddressingMode M>
void CPU::opAND()
{
	MemoryAccess src = getMemory<M>();
	registers.a &= src;
	setSign(registers.a);
	setZero(registers.a);
}

template <MemoryAddressingMode M>
void CPU::opASL()
{
	MemoryAccess src = getMemory<M>();
	registers.p.carry = (src & BIT_7);
	src = (src << 1) & 0xfe;
	setSign(src);
	setZero(src);
}

void CPU::opASLAccumulator()
{
	registers.p.carry = (registers.a & BIT_7);
	registers.a = (registers.a << 1) & 0xfe;
	setSign(registers.a);
	setZero(registers.a);
}

void CPU::opBCC()
{
	uint16_t address = registers.pc.w + (int8_t)getImmediate8() + 1;
	if( !registers.p.carry )
	{
		registers.pc.w = address;
	}
}

void CPU::opBCS()
{
	uint16_t address = registers.pc.w + (int8_t)getImmediate8() + 1;
	if( registers.p.carry )
	{
		registers.pc.w = address;
	}
}

void CPU::opBEQ()
{
	uint16_t address = registers.pc.w + (int8_t)getImmediate8() + 1;
	if( registers.p.zero )
	{
		registers.pc.w = address;
	}
}

template <MemoryAddressingMode M>
void CPU::opBIT()
{
	MemoryAccess src = getMemory<M>();
	setSign(src);
	registers.p.overflow = (0x40 & src);
	setZero(src & registers.a);
}

void CPU::opBNE()
{
	uint16_t address = registers.pc.w + (int8_t)getImmediate8() + 1;
	if( !registers.p.zero )
	{
		registers.pc.w = address;
	}
}

void CPU::opBPL()
{
	uint16_t address = registers.pc.w + (int8_t)getImmediate8() + 1;
	if( !registers.p.sign )
	{
		registers.pc.w = address;
	}
}

void CPU::opCLC()
{
	registers.p.carry = 0;
}

void CPU::opCLD()
{
	registers.p.decimal = 0;
}

template <MemoryAddressingMode M>
void CPU::opCMP()
{
	MemoryAccess src = getMemory<M>();
	uint8_t value = registers.a - src;
	registers.p.carry = ((registers.a >= src) ? 1 : 0);
	setSign(value);
	setZero(value);
}

template <MemoryAddressingMode M>
void CPU::opCPX()
{
	MemoryAccess src = getMemory<M>();
	uint8_t value = registers.x - src;
	registers.p.carry = ((registers.x >= src) ? 1 : 0);
	setSign(value);
	setZero(value);
}

template <MemoryAddressingMode M>
void CPU::opCPY()
{
	MemoryAccess src = getMemory<M>();
	uint8_t value = registers.y - src;
	registers.p.carry = ((registers.y >= src) ? 1 : 0);
	setSign(value);
	setZero(value);
}

template <MemoryAddressingMode M>
void CPU::opDEC()
{
	MemoryAccess src = getMemory<M>();
	src = src - 1;
	setSign(src);
	setZero(src);
}

void CPU::opDEX()
{
	registers.x--;
	setSign(registers.x);
	setZero(registers.x);
}

void CPU::opDEY()
{
	registers.y--;
	setSign(registers.y);
	setZero(registers.y);
}

template <MemoryAddressingMode M>
void CPU::opEOR()
{
	MemoryAccess src = getMemory<M>();
	registers.a ^= src;
	setSign(registers.a);
	setZero(registers.a);
}

template <MemoryAddressingMode M>
void CPU::opINC()
{
	MemoryAccess src = getMemory<M>();
	src = src + 1;
	setSign(src);
	setZero(src);
}

void CPU::opINX()
{
	registers.x++;
	setSign(registers.x);
	setZero(registers.x);
}

void CPU::opINY()
{
	registers.y++;
	setSign(registers.y);
	setZero(registers.y);
}

template <MemoryAddressingMode M>
void CPU::opJMP()
{
	MemoryAccess dest = getMemory<M>();
	registers.pc.w = dest.getAddress();
}

void CPU::opJSR()
{
	uint16_t address = getImmediate16();

	registers.pc.w--;
	push(registers.pc.h);
	push(registers.pc.l);
	registers.pc.w = address;
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

template <MemoryAddressingMode M>
void CPU::opLDY()
{
	MemoryAccess src = getMemory<M>();
	setSign(src);
	setZero(src);
	registers.y = src;
}

template <MemoryAddressingMode M>
void CPU::opLSR()
{
	MemoryAccess src = getMemory<M>();
	registers.p.sign = 0;
	registers.p.carry = (src & BIT_0);
	src = (src >> 1) & 0x7f;
	setZero(src);
}

void CPU::opLSRAccumulator()
{
	registers.p.sign = 0;
	registers.p.carry = (registers.a & BIT_0);
	registers.a = (registers.a >> 1) & 0x7f;
	setZero(registers.a);
}

template <MemoryAddressingMode M>
void CPU::opORA()
{
	MemoryAccess src = getMemory<M>();
	registers.a |= src;
	setSign(registers.a);
	setZero(registers.a);
}

void CPU::opPHA()
{
	push(registers.a);
}

void CPU::opPHP()
{
	push(registers.p.raw | 0x10);
}

void CPU::opPLA()
{
	registers.a = pull();
	setSign(registers.a);
	setZero(registers.a);
}

void CPU::opPLP()
{
	registers.p.raw = ((pull() & 0xef) | 0x20);
}

template <MemoryAddressingMode M>
void CPU::opROL()
{
	MemoryAccess src = getMemory<M>();
	bool bit7 = src & BIT_7;
	src = src << 1;
	if( registers.p.carry )
	{
		src = src | BIT_0;
	}
	registers.p.carry = (bit7 ? 1 : 0);
	setSign(src);
	setZero(src);
}

void CPU::opROLAccumulator()
{
	bool bit7 = registers.a & BIT_7;
	registers.a <<= 1;
	if( registers.p.carry )
	{
		registers.a |= BIT_0;
	}
	registers.p.carry = (bit7 ? 1 : 0);
	setSign(registers.a);
	setZero(registers.a);
}

template <MemoryAddressingMode M>
void CPU::opROR()
{
	MemoryAccess src = getMemory<M>();
	bool bit0 = src & BIT_0;
	src = src >> 1;
	if( registers.p.carry )
	{
		src = src | BIT_7;
	}
	registers.p.carry = (bit0 ? 1 : 0);
	setSign(src);
	setZero(src);
}

void CPU::opRORAccumulator()
{
	bool bit0 = registers.a & BIT_0;
	registers.a >>= 1;
	if( registers.p.carry )
	{
		registers.a |= BIT_7;
	}
	registers.p.carry = (bit0 ? 1 : 0);
	setSign(registers.a);
	setZero(registers.a);
}

void CPU::opRTS()
{
	uint16_t address = pull();
	address |= ((uint16_t)pull() << 8);
	registers.pc.w = address + 1;
}

template <MemoryAddressingMode M>
void CPU::opSBC()
{
	MemoryAccess src = getMemory<M>();
	uint16_t temp = registers.a - src - (registers.p.carry ? 0 : 1);
	setSign(temp);
	setZero(temp & 0xff);
	registers.p.overflow = (((registers.a ^ temp) & 0x80) && ((registers.a ^ src) & 0x80));
	if( registers.p.decimal )
	{
		if( ((registers.a & 0xf) - (registers.p.carry ? 0 : 1)) < (src & 0xf))
		{
			temp -= 6;
		}
		if( temp > 0x99 )
		{
			temp -= 0x60;
		}
	}
	registers.p.carry = (temp < 0x100);
	registers.a = (temp & 0xff);
}

void CPU::opSEC()
{
	registers.p.carry = 1;
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

template <MemoryAddressingMode M>
void CPU::opSTX()
{
	MemoryAccess dest = getMemory<M>();
	dest = registers.x;
}

template <MemoryAddressingMode M>
void CPU::opSTY()
{
	MemoryAccess dest = getMemory<M>();
	dest = registers.y;
}

void CPU::opTAX()
{
	registers.x = registers.a;
	setSign(registers.x);
	setZero(registers.x);
}

void CPU::opTAY()
{
	registers.y = registers.a;
	setSign(registers.y);
	setZero(registers.y);
}

void CPU::opTSX()
{
	registers.x = registers.s;
	setSign(registers.x);
	setZero(registers.x);
}

void CPU::opTXA()
{
	registers.a = registers.x;
	setSign(registers.a);
	setZero(registers.a);
}

void CPU::opTXS()
{
	registers.s = registers.x;
}

void CPU::opTYA()
{
	registers.a = registers.y;
	setSign(registers.a);
	setZero(registers.a);
}
