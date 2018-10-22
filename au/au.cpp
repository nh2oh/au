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
#include "aulib\contigmap.h"
#include "aulib\svector.h"

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

	sv::vector<int> sv1; //{1,2,3,4,5,7,8,9,11,-1,-6};
	sv1.push_back(3);
	sv1.push_back(7);


	contigmap<int,double> m1;
	m1.insert({0,1.0/1});
	m1.insert({2,1.0/2});
	m1.insert({17,1.0/17});
	std::cout << m1.print() << std::endl << std::endl;
	std::cout << "std::to_string(m1[2]) = " << std::to_string(m1[2]) << std::endl;
	m1[2] = 5.0;
	std::cout << "std::to_string(m1[2]) = " << std::to_string(m1[2]) << std::endl;
	m1.erase(2);
	std::cout << m1.print() << std::endl << std::endl;
	m1.insert({2,1.0/2});
	std::cout << m1.print() << std::endl << std::endl;

	auto b = m1.begin();
	auto bdr = *b;
	auto ppb = ++b;
	auto ppbdr = *ppb;

	for (auto e : m1) {
		auto x = e;
		std::cout <<"\t"<< e.k << " => " << e.v << std::endl ;
	}

	m1[2];
	m1[3];
	std::cout << m1.print() << std::endl << std::endl;
	m1[4] = 22.345;
	std::cout << m1.print() << std::endl << std::endl;

	/*
	diatonic_spn12tet sc {ntl_t{"C"},diatonic_spn12tet::mode::minor};
	std::vector<int> nts_scds {0,1,2,3,4,5,6};
	std::vector<ntstr_t> nts_ntstrs {};
	for (auto e : nts_scds) {
		nts_ntstrs.push_back(sc.to_ntstr(scd_t{e}));
	}
	rp_t rp {ts_t{4_bt,d::q},std::vector<d_t>(nts_ntstrs.size(),d::q)};
	line_t line {nts_ntstrs,rp};

	auto res = ks_key(line,ks_key_params{0});

	std::cout << printm(res.all_scores,"%10.3f,") << std::endl << std::endl;
	*/




	/*
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
	*/
    return 0;
}

