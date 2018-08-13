#include "au_error.h"
#include <string>
#include <iostream>
#include <cstdlib>

void au_error(std::string msg) {
	std::cout << std::endl << std::endl << msg << std::endl;
	std::cout << "Exiting!" << std::endl;
	std::abort();
}


void au_assert(bool cond, std::string msg) {
	if (!cond) { au_error(msg); }
}

void au_assert(bool cond) {
	if (!cond) { au_error("Too lazy to write a message"); }
}

