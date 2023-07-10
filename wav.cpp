#include "wav.h"

wav::wav() 
  : hdr()
  , samples()
  , mm(0.0f)
  , fail(false)
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

	mm = 1 / *std::max_element(samples.begin(), samples.end());

	file.close();
}

void wav::write(const std::string& filename) {
	std::ofstream file(filename, std::fstream::binary);
	file.write((char*)&hdr, 44);

	std::vector<int16_t> temp;
	temp.reserve(samples.size());

	for (float &sample : samples) {
		// sample * 32767
		temp.push_back((int16_t)(sample * ~-(1 << hdr.bitsPerSample - 1)));
	}

	file.write((char*)temp.data(), hdr.subchunk2Size);
	file.close();
}

void wav::write2(const std::string& filename, std::vector<float> data) {
	std::ofstream file(filename, std::fstream::binary);
	header cpy;
	std::memcpy(&cpy, &hdr, sizeof(hdr));

	cpy.subchunk2Size = data.size() * (cpy.bitsPerSample / 8);
	file.write((char*)&cpy, 44);

	std::vector<int16_t> temp;
	temp.reserve(data.size());

	for (float& sample : data) {
		// sample * 32767
		temp.push_back((int16_t)(sample * ~- (1 << cpy.bitsPerSample - 1)));
	}

	file.write((char*)temp.data(), cpy.subchunk2Size);
	file.close();
}

void wav::volume(float multiplier) {
	multiplier = std::clamp(multiplier, 0.0f, mm);
	for (float &s : samples) {
		s = std::clamp(s * multiplier, -1.0f, 1.0f);
	}
}

std::vector<float> wav::delay(float delay, float delayGain) {
	float sampleCount = std::round(hdr.sampleRate * delay);
	float sample, delayed;
	std::vector<float> temp;
	temp.reserve(samples.size() + sampleCount);

	for (int i = 0; i < samples.size() + sampleCount; i++) {
		if (i > sampleCount && i - sampleCount < samples.size()) {
			delayed = samples[i - sampleCount];
		} else {
			delayed = 0;
		}

		if (i < samples.size()) {
			sample = samples[i];
		} else {
			sample = 0;
		}

		temp.push_back(sample + delayGain * delayed);
	}
	return temp;
}
bool wav::ok() {
	return !fail;
}
