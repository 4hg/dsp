#include "wav.h"

wav::wav() 
  : hdr()
  , fail(false)
  , samples()
{}
wav::~wav() {}

void wav::read(const std::string& filename) {
	std::ifstream file(filename, std::fstream::binary);

	if (file.fail()) {
		std::cerr << "File failed to open.\n";
		fail = true;
		file.close();
		return;
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
	} else if (hdr.numChannels > 2) {
		std::cerr << "WAVE only has support for mono and stereo\n";
		fail = true;
	} else if (hdr.bitsPerSample != 16) {
		std::cerr << "Only pcm16le is supported.\n";
		fail = true;
	}

	if (fail) {
		file.close();
		return;
	}

	std::vector<uint8_t> temp(hdr.subchunk2Size);
	file.read((char*)temp.data(), hdr.subchunk2Size);

	int sampleSize = hdr.bitsPerSample / 8; // 2
	int length = hdr.subchunk2Size / sampleSize;
	
	samples.reserve(length);
	for (int i = 0; i < hdr.subchunk2Size; i += sampleSize) {
		// pcm16le
		int16_t sample = 0;
		for (int j = 0; j < sampleSize; j++) {
			sample |= temp[i + j] << j * 8;
		}
		// sample / 32768
		samples.push_back((float)sample / (1 << hdr.bitsPerSample - 1));
	}

	file.close();
}

void wav::write(const std::string& filename) {
	std::ofstream file(filename, std::fstream::binary);
	file.write((char*)&hdr, 44);

	std::vector<int16_t> temp;
	temp.reserve(samples.size());

	for (float &s : samples) {
		temp.push_back((int16_t)(s * 32768));
	}

	file.write((char*)temp.data(), hdr.subchunk2Size);
	file.close();
}

void wav::volume(float m) {
	for (float &s : samples) {
		s = std::clamp(s * m, -1.0f, 1.0f);
	}
}

bool wav::ok() {
	return !fail;
}
