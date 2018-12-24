#include "au_util.h"
#include <string>
#include <cmath>  // floor() in int_suffix()
#include <vector>
#include <chrono>  // for wait()


std::string int_suffix(int const& int_in) {
	std::string s {};
	if (int_in >= 11 && int_in <= 13) {
		s = "th";
	} else {
		int last_digit = int_in - static_cast<int>(std::floor(int_in/10))*10;
		switch (last_digit) {
		case 1:
			s = "st";
			break;
		case 2:
			s = "nd";
			break;
		case 3:
			s = "rd";
			break;
		default:
			s = "th";
			break;
		}
	}

	return s;
}


int wait() {
	std::chrono::microseconds wait_time {1};
	auto finish_time {std::chrono::system_clock::now()+wait_time};

	double keep_busy = 153625.2354;
	while (std::chrono::system_clock::now() < finish_time) {
		keep_busy += 1.0;
		keep_busy /= 3.0;
	}

	return static_cast<int>(keep_busy);
}




