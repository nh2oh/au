#pragma once
#include "uih.h"
#include <string>


struct parse_userinput_double {
	au::uih_parser_result<double> operator()(std::string const&) const;
};

