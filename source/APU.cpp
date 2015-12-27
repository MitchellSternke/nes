#include "APU.hpp"

uint8_t APU::readByte( uint16_t address )
{
	switch( address )
	{
	// Status
	case 0x4015:
		///@todo implement
		break;
	default:
		break;
	}

	return 0;
}

void APU::writeByte( uint16_t address, uint8_t value )
{
	switch( address )
	{
	// Pulse 1 control
	case 0x4000:
		///@todo implement
		break;
	// Pulse 1 sweep
	case 0x4001:
		///@todo implement
		break;
	// Pulse 1 timer, low bits
	case 0x4002:
		///@todo implement
		break;
	// Pulse 1 length counter load / timer high bits
	case 0x4003:
		///@todo implement
		break;
	// Pulse 2 control
	case 0x4004:
		///@todo implement
		break;
	// Pulse 2 sweep
	case 0x4005:
		///@todo implement
		break;
	// Pulse 2 timer, low bits
	case 0x4006:
		///@todo implement
		break;
	// Pulse 2 length counter load / timer high bits
	case 0x4007:
		///@todo implement
		break;
	// Triangle control
	case 0x4008:
		///@todo implement
		break;
	// Triangle timer, low bits
	case 0x400a:
		///@todo implement
		break;
	// Triangle length counter load / timer high bits
	case 0x400b:
		///@todo implement
		break;
	// Noise control
	case 0x400c:
		///@todo implement
		break;
	// Noise loop noise / noise period
	case 0x400e:
		///@todo implement
		break;
	// Noise length counter load
	case 0x400f:
		///@todo implement
		break;
	// DMC control
	case 0x4010:
		///@todo implement
		break;
	// DMC load counter
	case 0x4011:
		///@todo implement
		break;
	// DMC sample address
	case 0x4012:
		///@todo implement
		break;
	// DMC sample length
	case 0x4013:
		///@todo implement
		break;
	// Status
	case 0x4015:
		///@todo implement
		break;
	// Frame counter
	case 0x4017:
		///@todo implement
		break;
	default:
		break;
	}
}
