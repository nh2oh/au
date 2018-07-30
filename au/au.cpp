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
	auto ntfl = read_notefile(fname);


	scale_12tet sc {};
	std::vector<double> dt = {};
	std::vector<ntstr_t> nts {};
	for (auto e : ntfl) {
		auto curr_dt = (e.offtime-e.ontime)/1000;
		dt.push_back(curr_dt);

		nts.push_back(sc.to_ntstr(scd_t{e.pitch}));
	}

	auto fa4 = sc.to_frq(ntstr_t{"A"_ntl,4});
	auto f = sc.to_frq(ntstr_t{"C"_ntl,4});
	auto s = sc.to_scd(ntstr_t{"C"_ntl,4});
	auto ntstr60 = sc.to_ntstr(scd_t{60});
	auto ntstr48 = sc.to_ntstr(scd_t{48});


	std::cout << deltat2rp_demo() << std::endl << "----------------------" << std::endl;
	std::cout << deltat2rp_demo() << std::endl << "----------------------" << std::endl;
	std::cout << deltat2rp_demo() << std::endl << "----------------------" << std::endl;

    return 0;
}

