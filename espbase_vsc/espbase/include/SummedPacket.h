#ifndef _SUMMEDPACKET_H_
#define _SUMMEDPACKET_H_
#include <Arduino.h>

class SummedPacket
{
private:
	int packSize;
	int actualDataSize;
	uint8_t* internalPack;
public:	
	SummedPacket(int packSize);
	SummedPacket(int packSize, uint8_t* source);

	int GetSendSize();
	uint8_t* Data();
	bool IsValid();
};
#endif