#include "Arithmetic Coding.h"

//Thresholds for the AC algorithm
int AC::ACValueBits = 32;
uint64_t AC::max = (unsigned long long(1) << AC::ACValueBits) - 1;//high value of range, can only decrease
uint64_t AC::oneFourth = (AC::max + 1) / 4;
uint64_t AC::half = AC::oneFourth * 2;
uint64_t AC::threeFourth = AC::oneFourth * 3;


void AC::putBitPlusPending(bool bit, int & pending)
{
	BitIO::writeBit(bit, 1);
	for (int i = 0;i < pending;i++) {
		BitIO::writeBit(!bit, 1);
	}
	pending = 0;
}


//method to encode any text given by seq with AC. The data is writen bitwise to the output file
void AC::encodeAC(unsigned char * seq, int inputSize)
{
	//creating frequency/Interval Table : The model
	std::map<unsigned char, std::pair<uint64_t, uint64_t>> freq;// key -> (count of key, lower bound Interval)
	for (int i = 0; i < inputSize; i++) {
		freq[seq[i]].first += 1; //count the occurences of the key
	}

	uint64_t count = 0;
	for (auto it = freq.begin(); it != freq.end(); it++) {//calculate the probabilities and size of interval
		(it->second).second = count;
		count += ((it->second).first);// integer arithmetic, so don't map to [0,1)
	}

	//saving the model for later decode
	uint64_t freqSize = freq.size();
	BitIO::writeBit(freqSize, 64);

	for (auto it = freq.begin(); it != freq.end(); it++) {
		unsigned char key = it->first;
		uint64_t prob = (it->second).first;

		BitIO::writeBit(key, 8);//save key 

		//safe space by eencoding needed bits
		uint8_t probLen = 0;
		uint64_t probCopy = prob;
		while (probCopy > 0) {
			probCopy >>= 1;
			probLen++;
		}
		if (probLen == 0)
			probLen = 1;

		BitIO::writeBit(probLen, 6);
		BitIO::writeBit(prob, probLen);//save prob
	}

	BitIO::writeBit(inputSize, 64); //save the number of elements to write

	//after model is built and safed, encode input
	int pendingBits = 0; //Counter to store pending bits when low and high are converging
	uint64_t low = 0; //low limit for interval, can only increase
	uint64_t high = max; //high limit for interval, can only decrease


	for (int i = 0;i < inputSize; i++) {
		unsigned char cur = seq[i];

		uint64_t probHigh = freq[cur].second + freq[cur].first;//gets top interval Value
		uint64_t probLow = freq[cur].second;//gets bottom interval value

		uint64_t newRange = high - low + 1; //current subset
		high = low + (newRange*probHigh / inputSize) - 1; //progressive subdividing of range TODO groesse des ganzen arrays benutzen
		low = low + (newRange*probLow / inputSize);

		while (true) {

			if (high < half) {//MSB==0
				putBitPlusPending(0, pendingBits);
			}
			else if (low >= half) {//MSB==1
				putBitPlusPending(1, pendingBits);
			}
			else if (low >= oneFourth && high < threeFourth) {//converging
				pendingBits++;
				low -= oneFourth;
				high -= oneFourth;
			}
			else
			{
				break;
			}

			high <<= 1;
			high++;//high: never ending stream of ones, low: stream of zeroes
			low <<= 1;
			high &= max;
			low &= max;

		}

	}


	pendingBits++;
	if (low < oneFourth) {
		putBitPlusPending(0, pendingBits);
	}
	else {
		putBitPlusPending(1, pendingBits);
	}

	//Write trailing 0s
	BitIO::writeBit(0, ACValueBits - 2);

}


//method to decode AC text files. Result is a unsigned char pointer, safed in BitIO::pData as well as the size of the pointer in BitIO::pSize
void AC::decodeAC()
{
	//read and recreate the saved frequenzy table
	uint64_t freqSize = BitIO::readBit(64); //table size safed with 64 bit

	std::map<uint64_t, unsigned char> freq;//lower frequenzy -> key
	uint64_t count = 0;

	for (int i = 0;i < freqSize;i++) {
		unsigned char key = BitIO::readBit(8);
		uint64_t probLen = BitIO::readBit(6);
		uint64_t prob = BitIO::readBit(probLen);

		freq[count] = key;
		count += prob;
	}

	//read in previous number of encoded symbols
	uint64_t inputSize = BitIO::readBit(64);
	BitIO::pData = new unsigned char[inputSize]; //alloc memory
	BitIO::pSize = inputSize;

	freq[inputSize] = 0;//we need another upper bound

	//decoding
	uint64_t high = max;
	uint64_t low = 0;
	uint64_t val = 0; //encoded value to decode
	for (int i = 0;i < ACValueBits;i++) { //problem with little endian read in, instead shift all bit one by one
		val <<= 1;
		val += BitIO::readBit(1) ? 1 : 0;
	}

	int curPos = 0;
	while (true) {//run till all encoded vectors are decoded


			uint64_t range = high - low + 1;
			uint64_t scaledVal = ((val - low + 1)*inputSize - 1) / range;

			auto it = freq.upper_bound(scaledVal);
			uint64_t pHigh = it->first;//high bound of interval
			it--;
			uint64_t pLow = it->first;//low bound of interval

			BitIO::pData[curPos++] = it->second;

			high = low + (range*pHigh) / inputSize - 1;
			low = low + (range*pLow) / inputSize;

			while (true) {

				if (high < half) {//bit is 0

				}
				else if (low >= half) {
					val -= half;
					low -= half;
					high -= half;
				}
				else if (low >= oneFourth && high < threeFourth) {
					val -= oneFourth;
					low -= oneFourth;
					high -= oneFourth;
				}
				else {
					break;
				}

				low <<= 1;
				high <<= 1;
				high++;//high: never ending stream of ones
				val <<= 1;
				val += BitIO::readBit(1) ? 1 : 0;

			}


		if (curPos == inputSize) {//we have all our decoded symbols
			break;
		}

	}

}
