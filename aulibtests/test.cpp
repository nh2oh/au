#include "pch.h"
#include "..\aulib\types\nv_t.h"
#include <vector>
#include <string>
#include <cmath>

// Calling the constructor with no arguments is the same as calling w/
// {1,0}.  
TEST(nv_t_tests, constructor_zero_args) {
	nv_t nv_zeroargs {};
	nv_t nv_unity_nv {1,0};
	EXPECT_EQ(nv_zeroargs, nv_unity_nv);
}

// Tests operators <, >, ==, etc
TEST(nv_t_tests, gt_lt_ne_fullrange) {
	// All possible note-values from smallest to largest
	// Relies on my knowledge of the private exponent member constants
	int max_exp = 10; int min_exp = -3;
	int min_ndot = 0; int max_ndot = 5;
	std::vector<nv_t> nvs {};
	std::vector<int> nvs_ndots {};
	std::vector<double> nvs_bvs {};
	for (int m=max_exp; m>=min_exp; --m) {  // smallest bv -> largest bv
		for (int n=min_ndot; n<=max_ndot; ++n) {
			auto curr_bv = 1.0/std::pow(2,m);
			nvs.push_back(nv_t {curr_bv,n});
			nvs_ndots.push_back(n);
			nvs_bvs.push_back(curr_bv);
		}
	}

	// In-set comparisons
	for (int i=1; i<nvs.size(); ++i) {  // NB: starting @ the second element
		//EXPECT_NE(1.0,2.0);
		EXPECT_GT(nvs[i], nvs[i-1]) << nvs[i].print() << " > " << nvs[i-1].print();
		EXPECT_NE(nvs[i], nvs[i-1]);
		EXPECT_LT(nvs[i-1], nvs[i]);
		EXPECT_NE(nvs[i-1], nvs[i]);
	}
	
	// Comparisons to min and max nv_t's derived from the static member
	// constants
	nv_t nv_min {nv_t::min_bv,min_ndot};
	nv_t nv_max {nv_t::max_bv,max_ndot};
	for (int i=0; i<nvs.size(); ++i) {
		EXPECT_LE(nv_min,nvs[i]);
		EXPECT_GE(nv_max,nvs[i]);

		if (i == 0) {
			EXPECT_EQ(nv_min,nvs[i]);
		} else {
			EXPECT_GT(nvs[i],nv_min);
		}
		if (i == (nvs.size()-1)) {
			EXPECT_EQ(nv_max,nvs[i]);
		} else {
			EXPECT_LT(nvs[i],nv_max) << nvs[i].print() << " < " << nv_max.print();
		}
	}

	// Comparisons new objects constructed from the known bv, ndot values
	for (int i=0; i<nvs.size(); ++i) {
		auto curr_nv = nv_t {nvs_bvs[i],nvs_ndots[i]};
		EXPECT_EQ(nvs[i], curr_nv);
	}
	
}

TEST(nv_t_tests, constructor_quantization_bv) {
	// Random numbers in the range of allowed exponents
	std::vector<double> rexp {9.74,7.56,1.64,-2.36,8.44,4.92,-2.14,-0.02,5.38,
		1.77,4.17,6.69,4.36,9.05,2.19,-1.25,-1.36,4.99,3.08,4.29,4.48,-1.47,2.36,
		2.55,-1.26,-2.23,5.25,7.29,-1.8,2.46};
	for (int i=0; i<rexp.size(); ++i) {
		auto curr_bv_fuzz = 1.0/std::pow(2.0,rexp[i]);
		auto curr_exp_exact = static_cast<int>(std::round(rexp[i]));
		auto curr_bv_exact = 1.0/std::pow(2,curr_exp_exact);
		auto curr_nv_fuzz = nv_t {curr_bv_fuzz,0};
		auto curr_nv_exact = nv_t {curr_bv_exact,0};
		ASSERT_EQ(curr_nv_fuzz,curr_nv_exact) << curr_nv_fuzz.print() << 
			" == " << curr_nv_exact.print();

	}

}








