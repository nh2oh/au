#include <vector>
#include <string>
#include <iostream>
#include <numeric>
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


using namespace std::literals::string_literals;

int main(int argc) {

	float aa = 5.0;
	float yy = aa/3.0;

	if (aa == yy*(3.0)) {
		wait();
	} else {
		wait();
	}

	std::vector<note_value> nnvs {note_value{1.0/2.0},note_value{1.0/4.0},note_value{1.0/8.0}};
	double bpm = 60;
	ts_t ts = "4/4"_ts;

	auto rrp = rand_rp(ts,nnvs,std::vector<double>(nnvs.size(),1.0),0,bar_t{4});
	if (!rrp) {
		wait();
	}
	std::cout << printrp(ts,*rrp) << std::endl << std::endl;

	std::cout << tonset2rp_demo() << std::endl;

    return 0;
}

