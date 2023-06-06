#include "wav.h"

wav::wav() 
  : hdr()
  , fail(false)
  , samples()
{}
wav::~wav() {}

void wav::load(const std::string& filename) {
	std::ifstream file(filename, std::fstream::binary);

	if (file.fail()) {
		fail = true;
	}

	// read to audioFormat
	file.read((char*)&hdr, 22);
	
	if (hdr.subchunk1Size != 16 && hdr.audioFormat != 1) {
		std::cerr << "Non-PCM files not supported.\n";
		fail = true;
	}

	file.read(22 + (char*)&hdr, 22);

	if (hdr.chunkID[3] == 'X') {
		std::cerr << "BE byte ordering not supported.\n";
		fail = true;
	}

	std::vector<uint8_t> temp(hdr.subchunk2Size);
	file.read((char*)temp.data(), hdr.subchunk2Size);

	if (hdr.numChannels > 2) {
		std::cerr << "WAVE only has support for mono and stereo\n";
		fail = true;
	}

	int sampleSize = hdr.bitsPerSample / 8;
	int length = hdr.subchunk2Size / sampleSize;

	samples.resize(length);
	for (int i = 0; i < length; i += sampleSize) {
		uint32_t sample = 0;
		for (int j = 0; j < sampleSize; j++) {
			sample |= temp[i + j] << j * 8; 
		}
		float s = (float)sample / (1 << hdr.bitsPerSample - 1);
		samples.push_back(s);
	}

	file.close();
}

bool wav::ok() {
	return !fail;
}