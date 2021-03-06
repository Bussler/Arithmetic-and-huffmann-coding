#include <iostream>
#include "InputParser.h"
#include "Arithmetic Coding.h"
#include "HuffmanCoder.h"

using namespace std;

void writeDecodedToFile() {
	char ErgTxt[] = "decoded.txt";
	BitIO::openWrite(ErgTxt);
	for (int i = 0; i < BitIO::pSize; i++) {
		BitIO::writeBit(BitIO::pData[i], 8);
	}
	BitIO::writeRemainingBit();
	BitIO::closeWrite();
}

int main(int argc, char*argv[])
{
	if (argc > 4) {
		cout << "Too many arguments!" << endl;
		return 1;
	}

	char * txtName = argv[1];
	char * which = argv[2];
	char * what = argv[3];
	
	if (strcmp(which, "ac") == 0) {
		if (strcmp(what, "encoding") == 0) {
			BitIO::parseData(txtName);
			
			char txt[] = "encoded.txt";
			BitIO::openWrite(txt);
			cout << "Starting encoding of text" << endl;

			AC::encodeAC(BitIO::pData, BitIO::pSize);

			BitIO::writeRemainingBit();
			BitIO::closeWrite();
			
			cout << "Finished encoding of text in: encoded.txt" << endl;
		}
		else if (strcmp(what, "decoding") == 0) {
			BitIO::openRead(txtName);
			cout << "Starting decoding of text" << endl;
			AC::decodeAC();
			BitIO::closeRead();
			writeDecodedToFile();
			cout << "Finished decoding of text in: decoded.txt" << endl;
		}
	}
	else if (strcmp(which, "huffman") == 0) {
		if (strcmp(what, "encoding") == 0) {
			BitIO::parseData(txtName);

			char txt[] = "encoded.txt";
			BitIO::openWrite(txt);
			cout << "Starting encoding of text" << endl;

			HuffmanCoder coder;
			coder.encodeData(BitIO::pData, BitIO::pSize);

			BitIO::writeRemainingBit();
			BitIO::closeWrite();

			cout << "Finished encoding of text in: encoded.txt" << endl;

		}
		else if (strcmp(what, "decoding") == 0) {
			BitIO::openRead(txtName);
			cout << "Starting decoding of text" << endl;
			
			HuffmanCoder decoder;
			decoder.decodeData();

			BitIO::closeRead();
			writeDecodedToFile();
			cout << "Finished decoding of text in: decoded.txt" << endl;

		}
	}

	return 0;
}


