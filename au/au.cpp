#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <string>
#include "aulib\scale\scale.h"
#include "aulib\scale\scale_12tet.h"
#include "aulib\scale\scale_diatonic12tet.h"
#include "aulib\types\types_all.h"
#include "aulib\util\au_util_all.h"
#include "aulib\rpgen\rand_rp.h"
#include "line_t.h"
#include "aulib\input\notefile.h"
#include "aulib\types\nv_t.h"
#include "aulib\nv_uih.h"

using namespace std::literals::string_literals;

int main(int argc) {

	auto uihp1 = uih_pred([](std::string astr) {return (astr.size() != 0); },
		"ur strings too short");

	auto t1 = uihp1(""); auto m1=uihp1.msg();
	auto t2 = uihp1(std::string{""});
	auto t3 = uihp1("whatever");
	auto t4 = uihp1(std::string{"yAY"}); auto m4=uihp1.msg();

	wait();
/*	int max_exp = 10; int min_exp = -3;
	int min_ndot = 0; int max_ndot = 5;
	std::vector<nv_t> nvs {};
	std::vector<int> nvs_ndots {};
	std::vector<double> nvs_bvs {};
	std::vector<double> total_nvs {};
	for (int m=max_exp; m>=min_exp; --m) {  // smallest bv -> largest bv
		for (int n=min_ndot; n<=max_ndot; ++n) {
			auto curr_bv = 1.0/std::pow(2,m);
			auto a = nv_t::ndots_isvalid(n);
			auto b = nv_t::bv_isvalid(curr_bv);
			auto curr_nv = nv_t {curr_bv,n};
			auto curr_totalnv = (1.0/std::pow(2,m))*(2.0 - 1.0/std::pow(2,n));
			if (m==-1) {
				std::cout << "";
			}
			nvs.push_back(curr_nv);
			nvs_ndots.push_back(n);
			total_nvs.push_back(curr_totalnv);
			nvs_bvs.push_back(curr_bv);
			std::cout << "(m,n)==(" << m << "," << n << "):  " << curr_nv.print() << std::endl;
			if (nv_t::bv_isvalid(curr_totalnv)) {
				auto curr_nv_reverse = nv_t {curr_totalnv,0};
				std::cout << "\t" << curr_nv_reverse.print() << std::endl;
			} else {
				std::cout << "\t" << "too big!" << std::endl;
			}
		}
	}
	*/
	
    return 0;
}

