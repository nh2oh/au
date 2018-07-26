#include <string>
#include <iostream>
#include "au_error.h"


void au_error(std::string msg) {
	std::cout << std::endl << std::endl << msg << std::endl;
	std::cout << "Exiting!" << std::endl;
}
