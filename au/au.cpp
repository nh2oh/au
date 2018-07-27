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
#include "types\rp_t.h"
#include "rand_rp.h"
#include "line_t.h"


using namespace std::literals::string_literals;

int main(int argc) {
	//wrk edits
	std::vector<note_value> nnvs {note_value{1.0/2.0},note_value{1.0/4.0},note_value{1.0/8.0}};
	double bpm = 60;
	ts_t ts = "4/4"_ts;

	auto rrp = rand_rp(ts,nnvs,std::vector<double>(nnvs.size()-1,1.0),0,bar_t{4});
	if (!rrp) {
		wait();
	}
	std::cout << printrp(ts,*rrp) << std::endl << std::endl;

	auto new_rrp = *rrp;
	for (auto i=0; i<5; ++i) {
		auto t_on = rp2tonset(new_rrp,ts,bpm);
		auto t_delta = diffadj(t_on);
		t_delta = fuzzset(t_delta,0.1);
		for (auto j=1; j < t_delta.size(); ++j) {
			t_on[j] = t_on[j-1] + t_delta[j-1];
			if (t_on[j] <= t_on[j-1]) {
				wait();
			}
		}

		new_rrp = tonset2rp(t_on,ts,bpm,0.25);
		std::cout << printrp(ts,new_rrp) << std::endl << std::endl;
	}

	//auto s = tonset2rp_demo();
	//std::cout << s << std::endl;
    return 0;
}

