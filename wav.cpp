#include "wav.h"

wav::wav(const std::string filename) {
	setFields(filename);
}

wav::~wav() {}

void wav::setFields(const std::string filename) {
	std::ifstream file(filename, std::fstream::binary);
	char* buff;
	int length = 0;

	if (file.is_open()) {
		file.seekg(0, file.end);
		length = file.tellg();
		buff = new char[length];
		file.seekg(0, file.beg);
		file.read(buff, length);

		// do fancy work here
		std::cout << length;
		file.close();
		delete[] buff;
	}
}
