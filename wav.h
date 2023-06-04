#pragma once

#include <fstream>
#include <iostream>


class wav {
private:
	// struct for fields i suppose?

public:
	wav(std::string filename);
	~wav();

	void setFields(std::string filename);
};
