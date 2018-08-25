#pragma once
#include "nv_t_uih.h"
#include "types\nv_t.h"
#include "uih.h"
#include <string>
#include <optional>
#include "util\au_util.h"


using namespace au;

// TODOL  nv_t defines static members setting max and min base values
// ndot values.  This should check those.  

uih_parser_result<nv_t> 
	parse_userinput_nv::operator()(std::string const& str_in) const {
	uih_parser_result<nv_t> result {};

	auto o_matches = rx_match_captures("^\\s*(1/)?([1-9]+)(\\.*)?\\s*$",str_in);
	if (!o_matches || (*o_matches).size() != 4) {
		result.failmsg = "Parsing the nv failed for some reason... ";
		result.failmsg += "(*o_matches).size() == ";
		result.failmsg += std::to_string((*o_matches).size());
		return result;
	}

	auto matches = *o_matches;
	
	double bv = std::stod(*(matches[2]));
	if (matches[1]) {
		bv = 1.0/bv;
	}

	int ndots = 0;
	if (matches[3]) {
		ndots = (*(matches[3])).size();
	}

	result.o_result = nv_t {bv,ndots};
	result.failmsg.clear();

	return result;
}



