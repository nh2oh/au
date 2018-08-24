#pragma once
#include "..\types\nv_t.h"
#include "..\types\ts_t.h"
#include "..\types\beat_bar_t.h"
#include "..\uih.h"
#include <vector>
#include <chrono>
#include <string>
#include <set>

// TODO:  Not sure why i have to explictly define ==
struct rand_rp_opts {
	std::chrono::seconds maxt;
};

class randrp_input {
public:
	randrp_input() {
		nvset = std::set<nv_t> {};
		pd = std::vector<double> {};
	};
	randrp_input(ts_t ts, std::set<nv_t> nvset, std::vector<double> pd, int nnts,
		bar_t nbars) : ts{ts}, nvset{nvset}, pd{pd}, n_nts{nnts}, n_bars{nbars} {};

	bool operator==(randrp_input const& rhs) const {
		return (ts==rhs.ts && nvset==rhs.nvset && pd==rhs.pd &&
			n_nts == rhs.n_nts && n_bars == rhs.n_bars);
	};

	ts_t ts {};
	std::set<nv_t> nvset;
	std::vector<double> pd;
	int n_nts {};
	bar_t n_bars {};
};

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


std::optional<std::vector<nv_t>> rand_rp(randrp_input);
std::optional<std::vector<nv_t>> rand_rp(ts_t,std::vector<nv_t>,
	std::vector<double>,int,bar_t);
std::optional<std::vector<nv_t>> rand_rp(ts_t,std::vector<nv_t>,
	std::vector<double>,int,bar_t,rand_rp_opts);


