#include "wav.h"

int main() {
	wav w;
	w.read("audio/snare.wav");

	if (!w.ok())
		return 1;

	
	w.write2("audio/test.wav", w.delay(1.0f, 0.25f));
	return 0;
}
