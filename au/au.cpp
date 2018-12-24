#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <string>
#include <cstdio>
#include <cmath>
#include <any>
#include <tuple>
#include <type_traits>
#include <functional>
#include <optional>
#include <chrono>
#include <filesystem>
#include "..\aulib\aulib_all.h"
#include "dbklib\contigmap.h"
#include "dbklib\type_inspect.h"
#include "dbklib\stats.h"
#include "wavwr.h"
#include "dbklib\test_comp_fn.h"

size_t quantize_and_count_unique(
	std::vector<std::chrono::milliseconds> dt,
	std::chrono::milliseconds res, size_t error) {
	decltype(res/res) one = {1};
	for (auto& e : dt) {  // This loop assigns to dt!  e != const
		auto r = e/res;	
		// "round():" round up if e is exactly betweeen r*res and (r+1)*res;
		// round up if e < res => never round e to 0.  
		(e-r*res <= ((r+1)*res-e)) ? r=r : r=(r+1);  

		// "error = abs(e-e*res)"
		if (e > r*res) {
			error += (e-r*res).count();
		} else {
			error += (r*res-e).count();
		}
		e=(r*res);
	}

    std::sort(dt.begin(),dt.end());
    auto last = std::unique(dt.begin(),dt.end());
    return last-dt.begin();
}


int main(int argc) {


	auto re = new_randeng(true);
	std::uniform_int_distribution rd {};
	std::uniform_real_distribution rdd {};
	int retval {0};
	for (int i=0; i<50; ++i) {
		ma_params p {};
		rd.param(decltype(rd.param()) {10,100}); p.nnts = rd(re); //urandi(1,10,100)[0];
		rd.param(decltype(rd.param()) {50,200}); p.min = scd_t {rd(re)}; //scd_t{urandi(1,50,200)[0]};
		rd.param(decltype(rd.param()) {50,100}); p.max = p.min + scd_t {rd(re)}; //scd_t{urandi(1,50,100)[0]};
		rd.param(decltype(rd.param()) {1,1000}); p.npass = rd(re); //urandi(1,1,1000)[0];
		
		rdd.param(decltype(rdd.param()) {-10,10}); p.sc_adjnts = rdd(re); //urandd(1,-10,10)[0];
		p.sc_rptnts = rdd(re); //urandd(1,-10,10)[0];
		p.sc_stepsize = rdd(re); //urandd(1,-10,10)[0];
		rdd.param(decltype(rdd.param()) {0,10}); p.optimstep = rdd(re); //urandd(1,0,10)[0]; // should always be >= 0

		auto ma = melody_a(p);
		retval += ma[2];
	}

	return retval;




	/*

	//auto bf = readfile("C:\\Users\\ben\\Desktop\\cpp\\git\\au\\x64\\Debug\\rnts.wav");
	//auto riff = readriff(bf);

	auto x = test_comp_fn(77.3);

	melody_hiller_params p {};
	p.nnts = 16;
	p.max_rejects_tot = 100000;
	p.debug_lvl = 1;
	p.min = "C(3)";
	p.max = "C(5)";

	for (int i=0; i<5; ++i) {
		//std::cout << i << "\n";
		auto vnt = melody_hiller_ex21(p);
		std::cout << "r4 r4 r4 r4" << std::endl;
	}
	
    return 0;
	*/
}

