#include "InputParser.h"
#pragma warning(disable : 4996)


struct BitIO::RWWrapper {

	uint64_t wbyte = 0; //store for 8 byte
	uint64_t rbyte = 0;

	int numWBit = 63; //indicates free bit
	int numRBit = -1;

	int vergWBit = 0;
	int readRBit = 64;

	FILE * wFile;
	FILE * rFile;

}BitIO::rw;

void BitIO::writeData(unsigned char * data, int numBytes)
{
	fwrite(data, 1, numBytes, rw.wFile); //writes numBytes byte(char) into file
}

void BitIO::writeBit(uint64_t bits, int numBits)
{

	if (numBits + rw.vergWBit <= 64) {//we have free bit, just write them in
		rw.wbyte |= bits << (rw.vergWBit);
		rw.vergWBit += numBits;
	}
	else {// write as many bit as we can, store the rest again

		if (rw.vergWBit < 64) {//if there is still something free, squeeze it in
			rw.wbyte |= (bits << (rw.vergWBit));
		}

		writeData((unsigned char *)& rw.wbyte, sizeof(rw.wbyte));

		numBits -= 64 - rw.vergWBit;
		rw.wbyte = 0;
		rw.wbyte |= (bits >> (64 - rw.vergWBit));
		rw.vergWBit = 0 + numBits;
	}
}

void BitIO::writeRemainingBit()
{
	if (rw.vergWBit > 0) {
		writeData((unsigned char *)& rw.wbyte, sizeof(rw.wbyte));
	}

}

void BitIO::readData(uint8_t * buf, int numBytes)
{
	fread(buf, 1, numBytes, rw.rFile);
}

uint64_t BitIO::readBit(int numBits)
{
	uint64_t result = 0;
	if (numBits + rw.readRBit <= 64) {//we haven't read everything from the buffer
		int amtShift = 64 - numBits - rw.readRBit;
		result |= rw.rbyte << amtShift >> (amtShift + rw.readRBit);
		rw.readRBit += numBits;
	}
	else {
		if (rw.readRBit < 64) { //read as much as possible
			result |= rw.rbyte >> rw.readRBit;
		}

		readData((uint8_t *)& rw.rbyte, sizeof(rw.rbyte));

		numBits -= 64 - rw.readRBit;

		int amtShift = 64 - numBits - 0;
		result |= rw.rbyte << amtShift >> (amtShift + 0) << (64 - rw.readRBit);//shift to the amount of already read in data

		rw.readRBit = 0 + numBits;
	}

	return result;
}

void BitIO::openRead(char * name)
{
	rw.rFile = fopen(name, "rb");
}

void BitIO::closeRead()
{
	fclose(rw.rFile);
	rw.rbyte = 0;
	rw.readRBit = 64;
}

void BitIO::openWrite(char * name)
{
	rw.wFile = fopen(name, "wb");
}

void BitIO::closeWrite()
{
	fclose(rw.wFile);
	rw.wbyte = 0;
	rw.vergWBit = 0;
}

unsigned char * BitIO::pData;
int BitIO::pSize;

void BitIO::parseData(char * txtName)
{
	std::cout << "StartingParse" << std::endl;

	FILE *fp = fopen(txtName, "rb"); // open in binary
	if (fp == NULL) {
		std::cout << "Error opening file";
		exit(1);
	}

	fseek(fp, 0, SEEK_END);//get fileSize to allocate space
	pSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	pData = new unsigned char[pSize];
	fread((void*)pData, pSize, sizeof(unsigned char), fp);
	pData[pSize] = 0;//set eof here

	fclose(fp);

	/*std::ifstream in(txtName); // second option to parse whole txtFile with streams
	std::string contents((std::istreambuf_iterator<char>(in)),
		std::istreambuf_iterator<char>());

	std::cout << contents.c_str() << std::endl;*/

}
