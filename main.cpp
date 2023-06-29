#include "wav.h"

int main() {
	wav w;
	w.read("audio/440.wav");

	if (!w.ok())
		return 1;

	w.volume(4.0f);
	w.write("audio/test.wav");
	return 0;
}
