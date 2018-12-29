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

	melody_hiller_params p {};
	p.nnts = 16;
	p.nvoice = 2;
	p.max_rejects_tot = 100000;
	p.debug_lvl = 1;
	p.min = "C(3)";
	p.max = "C(5)";

	auto hrp = rhythm_hiller_ex32(p);
	auto dbkrp = hillerrp2dbkrp(hrp);

	for (int i=0; i<5; ++i) {
		//std::cout << i << "\n";
		auto vnt = melody_hiller_ex21(p);
		std::cout << "r4 r4 r4 r4" << std::endl;
	}
	
    return 0;

}

