#include "wav.h"

int main() {
	wav w;
	w.load("../../audio/440.wav");

	if (!w.ok())
		return 1;

	return 0;
}
