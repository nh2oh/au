#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <string>
#include <cstdio>
#include <any>
#include "aulib\scale\scales_all.h"
#include "aulib\types\types_all.h"
#include "aulib\util\au_util_all.h"
#include "aulib\rpgen\rand_rp.h"
#include "line_t.h"
#include "aulib\input\notefile.h"
#include "aulib\nv_uih.h"
#include "scr.h"


void printx() {
};

int main(int argc) {
	//std::string fmt {"yay:  1: %d, 2: %10.3f \n"};
	//std::cout << tf(fmt,5,432.3456) << std::endl;
	//printxx(fmt,5,432.3456);

	//auto x = tovectf(5,6,7,0,1);
	//auto z1 = kact<5> {};
	
	/*
	auto up11 = uih_pred {[](int const& i){return (i > 7);},
		"Must be > 7"};
	auto up12 = uih_pred {[](int const& i){return (i < 23);},
		"Must be < 23"};
	auto up2 = uih_pred {[](std::string const& s){return (s.size() < 13);},
		"s.size() must be <13"};

	auto up11t = up11.isvalid(5); auto up11m = up11.msg();
	auto up2t = up2.isvalid(std::string{"whatever"}); auto up2m = up2.msg();*/

	printx(1,2,3,4,5,"yay");

    return 0;
}

