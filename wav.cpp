#include "wav.h"

wav::wav(const std::string& filename) {
	setFields(filename);
}

wav::~wav() {}

wav::header wav::setFields(const std::string& filename) {
	std::ifstream file(filename, std::fstream::binary);
	header hdr{};

	if (file.fail())
		throw std::runtime_error("Could not open file.");
	
	file.read((char*)&hdr, 44);

	if (hdr.chunkID[3] == 'X') {
		std::cout << "BE byte ordering not supported.\n";
		exit(0);
	}

	if (hdr.audioFormat != 1) {
		std::cout << "Non-PCM files not supported.\n";
		exit(0);
	}

	file.close();
	return hdr;
}
