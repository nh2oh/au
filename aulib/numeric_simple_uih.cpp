#include "numeric_simple_uih.h"
#include "uih.h"
#include <string>
#include <regex>

using namespace au;

uih_parser_result<double> 
	parse_userinput_double::operator()(std::string const& str_in) const {

	std::regex rxp {"^\\s*(\\-)?([0-9]+)?(\\.)?([0-9]+)?\\s*$"};
	if (!std::regex_search(str_in,rxp)) {
		return uih_parser_result<double> {{},
			"Enter a number in the format: [-][\\d*][.][\\d*]"};
	}
	
	return uih_parser_result<double> {std::stod(str_in),""};
}





