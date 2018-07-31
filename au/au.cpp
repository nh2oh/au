#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <algorithm>
#include "scale.h"
#include "scale_12tet.h"
#include "scale_diatonic12tet.h"
#include "types\types_all.h"
#include "util\au_util_all.h"
#include "rand_rp.h"
#include "line_t.h"
#include "input\notefile.h"


using namespace std::literals::string_literals;

int main(int argc) {

	std::vector<note_value> nvs1 {"1/1"_nv, "1/2"_nv, "1/4."_nv, "1/4"_nv, "1/8"_nv};
	std::vector<beat_t> ph1(5,beat_t{0});
	tmetg_t mg1 = tmetg_t("4/4"_ts,nvs1,ph1);

	std::cout << mg1.print() << std::endl;
	mg1.set_rand_pg();

	std::cout << deltat2rp_demo() << std::endl << "----------------------" << std::endl;
	std::cout << deltat2rp_demo() << std::endl << "----------------------" << std::endl;
	std::cout << deltat2rp_demo() << std::endl << "----------------------" << std::endl;

    return 0;
}

