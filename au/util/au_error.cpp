#include <string>
#include <iostream>
#include <cstdlib>
#include <random>
#include <chrono>
#include "au_error.h"


void au_error(std::string msg) {
	std::cout << std::endl << std::endl << msg << std::endl;
	std::cout << "Exiting!" << std::endl;

	std::default_random_engine randeng {};
	randeng.seed(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> r {0,10};
	if (r(randeng) > 5) { 
		std::abort();
	}
}
