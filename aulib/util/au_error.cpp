#include "au_error.h"
#include <string>
#include <iostream>
#include <cstdlib>
#include <random>
#include <chrono>

//#include <boost\stacktrace.hpp>

void au_error(std::string msg) {
	std::cout << std::endl << std::endl << msg << std::endl;
	std::cout << "Exiting!" << std::endl;
	//std::cout << boost::stacktrace::stacktrace();
	std::default_random_engine randeng {};
	auto t = std::chrono::system_clock::now().time_since_epoch().count();
	randeng.seed(t);
	std::uniform_int_distribution<int> r {0,10};
	if (r(randeng) > 5) { 
		std::abort();
	}
}


void au_assert(bool cond, std::string msg) {
	if (!cond) { au_error(msg); }
}

void au_assert(bool cond) {
	if (!cond) { au_error("Too lazy to write a message"); }
}

