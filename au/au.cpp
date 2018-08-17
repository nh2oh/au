#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <string>
#include <cstdio>
#include <any>
#include <tuple>
#include <type_traits>
#include <functional>
#include <optional>
#include "aulib\aulib_all.h"
#include "scr.h"
#include "scr2.h"


int main(int argc) {
	


	struct someweirdstruct {
		std::string yay {"whatever"};
		double what {12343.34};
	};
	std::vector<double> vd {};
	decltype(vd)::value_type myd {};


	auto pf1 = uih_parser {[](std::string const& s){ return std::optional<int> {std::stoi(s)}; }};
	auto pf2 = uih_parser {[](std::string const& s){ return std::optional<someweirdstruct> {someweirdstruct{s,100.3}}; }};
	decltype(pf2)::RFType pf2rt {};


	auto up11 = uih_pred {[](int const& i){return (i > 7);},
		"Must be > 7"};
	auto up12 = uih_pred {[](int const& i){return (i < 23);},
		"Must be < 23"};
	auto up13 = uih_pred {[](int const& i){return (i != 14);},
		"Must != 14"};
	auto up2 = uih_pred {[](std::string const& s){return (s.size() < 13);},
		"s.size() must be <13"};


	auto up11t = up11.isvalid(5); auto up11m = up11.msg();
	auto up2t = up2.isvalid(std::string{"whatever"}); auto up2m = up2.msg();
	//auto x = validate_uihp(up11,up12);
	
	auto tsty1 = testyclass(pf1,up11,up12,up13);

	auto y = uih2(up11,up12, up13);
	y.update(13);

    return 0;
}

