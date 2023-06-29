#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>

class wav {
private:
	struct header {
		char     chunkID[4];
		uint32_t chunkSize;
		char     format[8];
		uint32_t subchunk1Size;
		uint16_t audioFormat;
		uint16_t numChannels;
		uint32_t sampleRate;
		uint32_t byteRate;
		uint16_t blockAlign;
		uint16_t bitsPerSample;
		char     subchunk2ID[4];
		uint32_t subchunk2Size;
	};
	header hdr;
	std::vector<float> samples;
	bool fail;

public:
	wav();
	~wav();

	void read(const std::string& filename);
	void write(const std::string& filename);

	void volume(float m);

	bool ok();
};
