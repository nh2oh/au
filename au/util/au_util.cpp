#include <string>
#include <cmath>
#include <vector>
#include <optional>
#include <regex>
#include "au_util.h"

int str2int(std::string s) {
	if (s.size() == 0) { return 0; }
	int sign = 1;
	int num = 0;
	size_t start_idx = 0;
	if (s.substr(start_idx,1) == "-" && s.size() == 1) {
		return 0;
	} else if (s.substr(start_idx,1) == "-" && s.size() > 1) {
		sign = -1;
		++start_idx;
	}

	num = std::stoi(s.substr(start_idx,s.size()));
	std::string str_from_num = std::to_string(num);
	if (str_from_num != s.substr(start_idx, s.size())) {
		return 0;
	}

	return (sign*num);
}


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
	std::string const& rx_exp, std::string const& subj) {
	std::regex rx(rx_exp);
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

int wait() {
	return 1;
}