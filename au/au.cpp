#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <string>
#include <cstdio>
#include <any>
#include <tuple>
#include <type_traits>
#include <functional>
#include <optional>
#include <chrono>
#include <filesystem>
#include "aulib\aulib_all.h"
#include "aulib\chunkyseq.h"



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

/*
	size_t rng_a {1103515245}; size_t rng_c {12345};  // Crude random numbers: (a*seed+c)%maxval

	size_t min_nbars = 1; size_t max_nbars = 5;
	for (int i=0; i<50; ++i) {
		size_t curr_nbars = (rng_a*i+rng_c)%(max_nbars+1);
		if (curr_nbars < min_nbars) { curr_nbars = min_nbars; }
		size_t curr_nnts = (rng_a*i+rng_c)%(8*curr_nbars+1);
		if (curr_nnts < 4*curr_nbars) { curr_nnts = 4*curr_nbars; }
		std::cout << "{"<<curr_nnts<<","<<curr_nbars<<"_br},";
	}

	*/

double n2f_eqt(int dn, double ref_frq, int ntet, int gint) {
	return ref_frq*std::pow(gint,dn/static_cast<double>(ntet));
}
double f2n_eqt(double frq_in, double ref_frq, int ntet, int gint) {
	return ntet*(std::log(frq_in/ref_frq)/std::log(static_cast<double>(gint)));
}


int main(int argc) {

	//int delta = 57;
	for (int i=-1000; i<1000; ++i) {
		//int new_i = i-delta;
		double f = n2f_eqt(i,440.0,12,2);
		double n = f2n_eqt(f,440.0,12,2);


		if (!aprx_int(n)) {
			wait();
		}
	}


	spn12tet sc {};
	std::cout << sc.print(scd_t{-13},scd_t{100}) << std::endl;;

	std::vector<double> s1 {1,2,3,4,5,6,7,8,9,10,11,12};
	std::vector<double> s2 {0,2,4,6,8,10,12,14};
	std::vector<double> s3 {1,3,5,7,9,11,13};
	std::vector<double> s4 {1,2,3};
	std::vector<double> s5 {5,6,7};
	chunkyseq<double> c1 {};
	std::vector<double> getres {};
	c1.push_back(s1);
	std::cout << c1.print() << std::endl;
	c1.push_back(s4); c1.push_back(1.3); c1.push_back(s5); c1.push_back(s4);
	std::cout << c1.print() << std::endl;
	c1.insert(1,2.2);
	std::cout << c1.print() << std::endl;
	getres = c1.get(2);
    return 0;

}

