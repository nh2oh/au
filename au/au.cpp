#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <string>
#include <cstdio>
#include <any>
#include <tuple>
#include "aulib\aulib_all.h"
#include "scr.h"

struct uihp_validator_result {
	bool isvalid {false};
	std::string msg {""};
};

bool validate_uihp() {
	return true;
};

template<typename T, typename... Tail>
bool validate_uihp(T head, Tail... tail) {
	return (head.isvalid(1) && validate_uihp(tail...));
};


int main(int argc) {
	//std::string fmt {"yay:  1: %d, 2: %10.3f \n"};
	//std::cout << tf(fmt,5,432.3456) << std::endl;
	//printxx(fmt,5,432.3456);

	//auto x = tovectf(5,6,7,0,1);
	//auto z1 = kact<5> {};
	
	
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
	auto x = validate_uihp(up11,up12);
	
	auto tt = std::tuple {up11, up12, up13};

	auto y = uih2(up11,up12, up13);
	y.update(-12);

    return 0;
}

