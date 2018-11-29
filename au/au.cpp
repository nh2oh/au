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

struct fprep {
	double s {0.0};
	int e {0};
};

union dblrep {
	double d;
	uint64_t i;
};

int main(int argc) {

	double a = 1.0/3.0;
	fprep a_fprep {};
	a_fprep.s = std::frexp(a,&(a_fprep.e));

	dblrep a_rep;  a_rep.d = a;  std::cout << a_rep.i << std::endl;
	dblrep b_rep;  b_rep.d = 1.0;  std::cout << b_rep.i << std::endl;
	dblrep c_rep;  c_rep.d = 3.0;  std::cout << c_rep.i << std::endl;
	dblrep d_rep;  d_rep.d = 0.0;  std::cout << d_rep.i << std::endl;
	dblrep e_rep;  e_rep.d = -0.0;  std::cout << e_rep.i << std::endl;


	/*
	ts_t ses {12_bt,d::edd,false};
	ts_t sec {12_bt,d::edd,true};

	std::cout << ses.print() << std::endl
		<< sec.print() << std::endl;
	*/

    return 0;

}

