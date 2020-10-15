#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "InputParser.h"

namespace AC {

	extern int ACValueBits;
	extern uint64_t max;
	extern uint64_t oneFourth;
	extern uint64_t half;
	extern uint64_t threeFourth;

	void putBitPlusPending(bool bit, int& pending); // helper method for AC algorithm

	void encodeAC(unsigned char* seq, int inputSize);
	void decodeAC();

}