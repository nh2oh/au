#include "au_util.h"
#include <string>
#include <cmath>  // floor() in int_suffix()
#include <vector>
#include <optional>
#include <regex>
#include <chrono>  // for wait()
//#include <cstdio> // sprintf()
//#include <cstdarg> // va_arg




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


std::optional<std::vector<std::optional<std::string>>> rx_match_captures(
	std::regex const& rx, std::string const& subj) {
	std::smatch rx_matches;

	if (!std::regex_match(subj, rx_matches, rx)) {
		return {};
	}

	std::vector<std::optional<std::string>> captures {};
	for (int i=0; i<rx_matches.size(); ++i) {
		std::ssub_match sub_match = rx_matches[i];
		if (sub_match.matched) {
			captures.push_back(sub_match.str());
		} else {
			captures.push_back({});
		}
	}

	return captures;
}

std::optional<std::vector<std::optional<std::string>>> rx_match_captures(
	std::string const& rx_exp, std::string const& subj) {
	std::regex rx(rx_exp);
	return rx_match_captures(rx,subj);
}

int wait() {
	std::chrono::microseconds wait_time {1};
	auto finish_time {std::chrono::system_clock::now()+wait_time};

	double keep_busy = 153625.2354;
	while (std::chrono::system_clock::now() < finish_time) {
		keep_busy += 1.0;
		keep_busy /= 3.0;
	}

	return 1;
}

