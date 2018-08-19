//#include <vector>
//#include <string>
//#include <iostream>
//#include <numeric>
//#include <algorithm>
//#include <string>
//#include <cstdio>
//#include <any>
//#include <tuple>
//#include <type_traits>
//#include <functional>
//#include <optional>
//#include "aulib\aulib_all.h"

//#include "aulib\uih.h"
//#include "aulib\types\ts_t_uih.h"
//#include "aulib\numeric_simple_uih.h"

int main(int argc) {
	/*
	uih_parser m_bpm_parser {parse_userinput_double {},
		"A number > 0 (decimals allowed)."};
	uih_pred bpm_pred_positive {[](double const& inp){return (inp > 0.0);},
		"Value must be > 0"};
	uih m_bpm_uih {m_bpm_parser, bpm_pred_positive};
	*/

	/*
	uih_parser_result res {std::optional<double> {}, std::string{""}};

	struct someweirdstruct {
		std::string yay {"whatever"};
		double what {12343.34};
	};


	auto pf1 = uih_parser {
		[](std::string const& s){ uih_parser_result res {std::optional<double> {}, std::string{""}}; res.o_result=std::stod(s); res.failmsg =""; return res;},
		"This parser converts a std::string to a double."};
	
	auto pf2 = uih_parser {
		[](std::string const& s){return uih_parser_result {std::optional<int>{std::stoi(s)}, std::string{""}};},
		"This parser converts a std::string to an int."};
	auto pf3 = uih_parser {
		[](std::string const& s){return uih_parser_result {std::optional<someweirdstruct>{someweirdstruct{s,100.3}}, std::string{""}};},
		"This parser converts a std::string to a someweirdstruct."};
	//decltype(pf2)::RFType pf2rt {};


	
	auto up11 = uih_pred {[](int const& i){return (i > 7);},
		"Must be > 7"};
	auto up12 = uih_pred {[](int const& i){return (i < 23);},
		"Must be < 23"};
	auto up13 = uih_pred {[](int const& i){return (i != 14);},
		"Must != 14"};
	auto up2 = uih_pred {[](std::string const& s){return (s.size() < 13);},
		"s.size() must be <13"};

	//auto up11t = up11.isvalid(5); auto up11m = up11.msg();
	//auto up2t = up2.isvalid(std::string{"whatever"}); auto up2m = up2.msg();
	//auto x = validate_uihp(up11,up12);
	
	auto tsty1 = testyclass(pf1,up11,up12,up13);
	typename decltype(tsty1)::PAType x1;
	auto tsty2 = testyclass(pf2,up11,up12,up13);
	typename decltype(tsty2)::PAType x2;

	tsty2.update(std::string{"10"});
	tsty2.update(std::string{"14"});

	*/

	/*
	uih_parser ts_parser {parse_userinput_ts {},
		"The format of a ts is n/d where n,d are both integers > 0."};

	uih_pred ts_pred_nocompound {
		[](ts_t_parts const& inp){return !(inp.is_compound);},
		"Compound time signatures are not yet supported."};

	uih_pred ts_pred_lownums {
		[](ts_t_parts const& inp){return (inp.bt_per_bar<13 && inp.nv_per_bt<13);},
		"Values of the numerator and denominator > 13 are not yet supported."};

	uih ts_uih1 {ts_parser,ts_pred_nocompound,ts_pred_lownums};

	ts_uih1.update("no way");
	std::cout << "1:  " << ts_uih1.is_valid() << ";  " << ts_uih1.msg() << std::endl;
	ts_uih1.update("4/4");
	std::cout << "2:  " << ts_uih1.is_valid() << ";  " << ts_uih1.msg() << std::endl;
	ts_uih1.update("6/8c");
	std::cout << "3:  " << ts_uih1.is_valid() << ";  " << ts_uih1.msg() << std::endl;
	ts_uih1.update("24/8c");
	std::cout << "4:  " << ts_uih1.is_valid() << ";  " << ts_uih1.msg() << std::endl;

	uih ts_uih2 {ts_parser};
	ts_uih2.update("4/4");
	std::cout << "2:1:  " << ts_uih2.is_valid() << ";  " << ts_uih2.msg() << std::endl;
	ts_uih2.update("24/8c");
	std::cout << "2:2:  " << ts_uih2.is_valid() << ";  " << ts_uih2.msg() << std::endl;
	ts_uih2.update("-4/4");
	std::cout << "2:3:  " << ts_uih2.is_valid() << ";  " << ts_uih2.msg() << std::endl;
	ts_uih2.update("4/-4");
	std::cout << "2:4:  " << ts_uih2.is_valid() << ";  " << ts_uih2.msg() << std::endl;
	*/
    return 0;
}

