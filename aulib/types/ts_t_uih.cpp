#pragma once
#include "ts_t_uih.h"
#include "ts_t.h"
#include "..\uih.h"
#include <string>
#include <optional>
#include "..\util\au_util.h"


using namespace au;

uih_parser_result<ts_t> 
	parse_userinput_ts::operator()(std::string const& str_in) const {
	uih_parser_result<ts_t> result {};

	auto o_matches = rx_match_captures("^\\s*([1-9]+)/([1-9]+)(c)?\\s*$",str_in);
	if (!o_matches || (*o_matches).size() != 4) {
		result.failmsg = "Parsing the ts failed for some reason... ";
		result.failmsg += "(*o_matches).size() == ";
		result.failmsg += std::to_string((*o_matches).size());
		return result;
	}

	auto matches = *o_matches;
	
	auto bt_per_bar = std::stod(*(matches[1]));
	auto nv_per_bt = 1.0/std::stod(*(matches[2]));
	bool is_compound = false;
	if (matches[3]) {
		bool is_compound = true;
	}

	result.o_result = ts_t {beat_t{bt_per_bar}, nv_t{nv_per_bt}, is_compound};
	result.failmsg.clear();

	return result;
}



