#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <algorithm>
#include "aulib\scale\scale.h"
#include "aulib\scale\scale_12tet.h"
#include "aulib\scale\scale_diatonic12tet.h"
#include "aulib\types\types_all.h"
#include "aulib\util\au_util_all.h"
#include "aulib\rpgen\rand_rp.h"
#include "line_t.h"
#include "aulib\input\notefile.h"

using namespace std::literals::string_literals;

int main(int argc) {

	std::vector<note_value> nvs1 {"1/1"_nv, "1/2"_nv, "1/4."_nv, "1/4"_nv, "1/8"_nv};
	std::vector<beat_t> ph1(5,beat_t{0});
	tmetg_t mg1 = tmetg_t("4/4"_ts,nvs1,ph1);

	std::cout << mg1.print() << std::endl;
	mg1.set_rand_pg();
	for (auto i=0; i<5; ++i) {
		auto rrp = mg1.draw();
		std::cout << std::endl << printrp("4/4"_ts,rrp) << std::endl;
	}


	std::vector<note_value> nvs2 {"1/1"_nv, "1/2"_nv, "1/4"_nv};
	std::vector<beat_t> ph2(3,beat_t{0});
	tmetg_t mg2 = tmetg_t("4/4"_ts,nvs2,ph2);
	std::cout << mg2.print() << std::endl;
	mg2.enumerate();


    return 0;
}

