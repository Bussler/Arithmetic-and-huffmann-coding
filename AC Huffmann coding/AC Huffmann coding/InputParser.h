#pragma once
#include <iostream>
#include <fstream>

namespace BitIO {

	extern struct RWWrapper rw; //wrapper class to hold information for read/write

	void writeData(unsigned char* data, int numBytes);
	void writeBit(uint64_t bits, int numBits);
	void writeRemainingBit();

	void readData(uint8_t * buf, int numBytes);
	uint64_t readBit(int numBits);

	void openRead(char* name);
	void closeRead();
	void openWrite(char* name);
	void closeWrite();

	extern unsigned char * pData;
	extern int pSize;
	void parseData(char * txtName);

}