#ifndef NES_HPP
#define NES_HPP

#include "APU.hpp"
#include "Controller.hpp"
#include "CPU.hpp"
#include "Memory.hpp"
#include "PPU.hpp"
#include "ROMImage.hpp"

/**
 * Interface for all NES emulation.
 */
class NES
{
public:
	NES( uint8_t* romData );

	APU& getAPU();
	Controller& getController1();
	Controller& getController2();
	CPU& getCPU();
	Memory& getMemory();
	PPU& getPPU();
	ROMImage& getROMImage();

	/**
	 * Step a single frame of emulation.
	 */
	void stepFrame();

private:
	ROMImage romImage;
	Memory memory;
	CPU cpu;
	PPU ppu;
	APU apu;
	Controller controller1;
	Controller controller2;
};

#endif // NES_HPP
