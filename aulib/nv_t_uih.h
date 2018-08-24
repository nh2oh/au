#pragma once
#include "uih.h"
#include "types\nv_t.h"
#include <string>


// My primary nv parser
struct parse_userinput_nv {
	using PIType = typename std::string const&;
	au::uih_parser_result<nv_t> operator()(std::string const&) const;
};
