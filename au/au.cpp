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
	std::vector<double> t_delta((*rrp).size()+1,0.0);
	std::vector<double> t_on {};
	auto new_rrp = *rrp;
	for (auto i=0; i<5; ++i) {
		t_on = rp2tonset(new_rrp,ts,bpm);
		//auto t_delta = diffadj(t_on);
		std::adjacent_difference(t_on.begin(),t_on.end(),t_delta.begin());
		t_delta = fuzzset(t_delta,0.01);
		t_on = vadd(t_on,t_delta);
		for (auto j=1; j<t_on.size(); ++j) {
			if (isapproxeq(t_on[j],t_on[j-1],3)) {
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

