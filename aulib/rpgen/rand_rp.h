#pragma once
#include "..\types\nv_t.h"
#include "..\types\ts_t.h"
#include "..\types\beat_bar_t.h"
#include "..\uih.h"
#include "..\types\rp_t.h"
#include <vector>
#include <chrono>
#include <string>
#include <set>
#include <optional>

struct rand_rp_opts {
	std::chrono::seconds maxt {3};
};

struct randrp_input {
	randrp_input()=default;
	ts_t ts {};
	std::set<d_t> nvset {};  // TODO:  srd::set => gross
	std::vector<double> pd {};
	int n_nts {};
	bar_t n_bars {};

	bool operator==(randrp_input const& rhs) const {
		return (ts==rhs.ts && nvset==rhs.nvset && pd==rhs.pd &&
			n_nts == rhs.n_nts && n_bars == rhs.n_bars);
	};
};


std::optional<rp_t> rand_rp(ts_t,std::vector<d_t>,
	std::vector<double>,int,bar_t);

std::optional<rp_t> rand_rp(ts_t,std::vector<d_t>,
	std::vector<double>,int,bar_t,rand_rp_opts);

std::optional<rp_t> rand_rp(randrp_input);


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

