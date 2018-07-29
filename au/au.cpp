#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <algorithm>
#include "scale.h"
#include "scale_12tet.h"
#include "scale_diatonic12tet.h"
#include "types\frq_t.h"
#include "types\scd_t.h"
#include "types\ntl_t.h"
#include "util\au_util_all.h"
#include "types\beat_bar_t.h"
#include "types\rp_t.h"
#include "types\ts_t.h"
#include "types\note_value_t.h"
#include "types\metg_t.h"
#include "rand_rp.h"
#include "line_t.h"
#include "input\notefile.h"


using namespace std::literals::string_literals;

int main(int argc) {

	std::string fname {};
	fname += "C:\\Users\\ben\\Desktop\\music\\materials_temperley\\";
	fname += "essen-perf\\france01.notes";

	double min_dt = 100000;
	std::vector<double> dt = {};
	auto nts = read_notefile(fname);
	for (auto e : nts) {
		auto curr_dt = (e.offtime-e.ontime)/1000;
		dt.push_back(curr_dt);
		if (curr_dt < min_dt) {
			min_dt = curr_dt;
		}
	}
	
	std::vector<double> dtrats {};
	for (auto e : dt) {
		dtrats.push_back(static_cast<double>(e)/min_dt);
	}

	//auto rp = deltat2rp(dt,"4/4"_ts,200,0.1);
	//std::cout << printrp("4/4"_ts,rp) << std::endl;
	
	std::cout << deltat2rp_demo() << std::endl << "----------------------" << std::endl;
	std::cout << deltat2rp_demo() << std::endl << "----------------------" << std::endl;
	std::cout << deltat2rp_demo() << std::endl << "----------------------" << std::endl;

    return 0;
}

