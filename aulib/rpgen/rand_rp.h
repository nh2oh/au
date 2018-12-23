#pragma once
#include "..\types\nv_t.h"
#include "..\types\ts_t.h"
#include "..\types\beat_bar_t.h"
#include "..\types\rp_t.h"
#include <vector>
#include <chrono>

struct randrp_input {
	ts_t ts {};
	std::vector<d_t> nvset {};
	std::vector<double> pd {};
	int nnts {0};
	bar_t nbars {0};
	std::chrono::seconds maxt {3};  // timeout

	bool operator==(randrp_input const& rhs) const {
		return (ts==rhs.ts && nvset==rhs.nvset && pd==rhs.pd &&
			nnts == rhs.nnts && nbars == rhs.nbars);
		// TODO:  chck maxt
		// TODO:  Also == if the elements of nvset are the same but out of order
	};
};

struct randrp_result {
	bool success {false};
	rp_t rp {};
};

randrp_result rand_rp(const randrp_input&);
randrp_result rand_rp(ts_t,std::vector<d_t>,std::vector<double>,int,bar_t,std::chrono::seconds);


struct rrpinput_validator_result {
	bool input_is_valid {false};
	bool nvset_is_valid {false};
	bool pd_is_valid {false};
	bool nnts_is_valid {false};
	bool nbars_is_valid {false};
	bool maxt_is_valid {false};

	std::string msg {};
};

rrpinput_validator_result validate_randrp_input(const randrp_input&);


/*
//
// My randrp_input parser
// As w/ all uih parsers, returns a uih_parser_result struct. Member 
// o_result is a std::optional<randrp_input>.   If there is some problem 
// w/ the input, you can inspect member .failmsg (set by operator()) to
// find out why.  
// 
// Construct like:
// 	au::uih_parser<parse_randrp_input> my_parser {parse_randrp_input {},
//    "Here is some helpful info about rand_rp()"};
// ... Then add additional predicates as needed.
// 
struct parse_randrp_input {
	using PIType = typename randrp_input const&;
	au::uih_parser_result<randrp_input> operator()(randrp_input const&) const;
};

*/


