#include "numeric_simple_uih.h"
#include "uih.h"
#include <string>
#include <regex>

using namespace au;

// TODO:  std::stod() simply crashes if you give it certain invalid
// inputs, ex, '.'
// TODO:  Also crashes if the input is just: "-"
uih_parser_result<double> 
	parse_userinput_double::operator()(std::string const& str_in) const {

	//std::regex rxp {"^\\s*(\\-)?([0-9]+)?(\\.)?([0-9]+)?\\s*$"};
	std::regex rxp {"^\\s*\\-?(?:[0-9]+\\.?[0-9]*)|(?:[0-9]*\\.?[0-9]+)\\s*$"};
	if (!std::regex_search(str_in,rxp)) {
		return uih_parser_result<double> {{},
			"Enter a number in the format: [-][\\d*][.][\\d*]"};
	}
	
	return uih_parser_result<double> {std::stod(str_in),""};
}


uih_parser_result<int> 
parse_userinput_int::operator()(std::string const& str_in) const {

	//std::regex rxp {"^\\s*(\\-)?([0-9]+)\\s*$"};
	std::regex rxp {"^\\s*\\-?[0-9]+\\s*$"};
	if (!std::regex_search(str_in,rxp)) {
		return uih_parser_result<int> {{},
			"Enter a number in the format: [-]\\d*"};
	}

	return uih_parser_result<int> {std::stoi(str_in),""};
}


