#include "pch.h"
#include "..\aulib\types\nv_t.h"
#include <vector>
#include <string>
#include <cmath>

// 
TEST(d_t_tests, AssortedConstructorTests) {

	// Calling the constructor with no arguments creates a duration of 0
	d_t nv_zeroargs {};
	EXPECT_TRUE((nv_zeroargs + d_t{d::w}) == d_t{d::w});

	// From double's... a random collection
	EXPECT_TRUE(d_t{0.25} == d_t{d::q});
	EXPECT_TRUE(d_t{0.125} == d_t{d::e});
	EXPECT_TRUE(d_t{0.25+0.125} == d_t{d::qd});
}

// Tests operators <, >, ==, etc
TEST(d_t_tests, OperatorsGtLtEqNeqEtc) {
	// From short durations -> long durations
	auto nvs = autests::make_dt_set(-3,0,12,5);

	// Comparison operators
	for (int i=1; i<nvs.size(); ++i) {  // NB: starting @ the second element
		EXPECT_TRUE(nvs[i].dt != nvs[i-1].dt);
		EXPECT_TRUE(nvs[i].dt == nvs[i].dt);
		EXPECT_TRUE(nvs[i-1].dt < nvs[i].dt);
		EXPECT_TRUE(nvs[i-1].dt <= nvs[i].dt);
		EXPECT_FALSE(nvs[i-1].dt >= nvs[i].dt);
		EXPECT_FALSE(nvs[i].dt > nvs[i].dt);
	}
}

// Tests the getters
TEST(d_t_tests, MethodsBaseAndNdots) {
	// From short durations -> long durations
	auto nvs = autests::make_dt_set(-3,0,12,5);

	// singlet_exists() == true for all the notes of the set
	// Also tests getter methdos ndots(), base()
	for (int i=0; i<nvs.size(); ++i) {
		EXPECT_TRUE(nvs[i].dt.singlet_exists());
		EXPECT_TRUE(nvs[i].dt.ndot() == nvs[i].n);
		EXPECT_TRUE(nvs[i].dt.base() == nvs[i].m);
	}
}


// 
TEST(d_t_tests, MethodToSinglets) {
	// From short durations -> long durations
	auto nvs = autests::make_dt_set(-3,0,12,5);

	// Since each element of the set is a singlet, to_singlets() should just
	// return the d_t itself in a vector of size == 1
	for (int i=0; i<nvs.size(); ++i) {
		auto curr_singlets = nvs[i].dt.to_singlets();
		EXPECT_TRUE(curr_singlets.size() == 1);
		EXPECT_TRUE(curr_singlets[0] == nvs[i].dt);
		EXPECT_TRUE(curr_singlets[0].base() == nvs[i].m);
		EXPECT_TRUE(curr_singlets[0].ndot() == nvs[i].n);
	}
}

// 
TEST(d_t_tests, OperatorsPlusMinus) {
	// From short durations -> long durations
	auto nvs = autests::make_dt_set(-3,0,12,5);

	// Since each element of the set is a singlet, to_singlets() should just
	// return the d_t itself in a vector of size == 1
	for (auto e : nvs) {
		// For a note a w/ ma, na>0, subtracting a note of the same m and i<n dots
		// should yield a note b with mb = ma-(i+1) and nb = na - (i+1)
		// EX:  qdddd = (1/4) + (1/8) + (1/16) + (1/32) + (1/64)
		//     -qdd   = (1/4) + (1/8) + (1/16)
		//  => i=2dots => i+1 = 3 => mb = -2 - (3) = -5, nb = 4-3 = 1
		//            =                          (1/32) + (1/64)
		for (auto i=0; i<e.n; ++i) {  // Loop does not run for e.n == 0
			auto x = e.dt - d_t{d_t::mn{e.m,i}};
			auto y = d_t {d_t::mn{e.m+i+1,e.n-(i+1)}};
			EXPECT_TRUE(x == y);
		}

	}
}

// 
TEST(d_t_tests, DivisionAndConstructFromDouble) {
	// From short durations -> long durations
	auto nvs = autests::make_dt_set(-3,0,12,5);

	// Since each element of the set is a singlet, to_singlets() should just
	// return the d_t itself in a vector of size == 1
	for (auto e : nvs) {
		double d = e.dt/d_t{d::w};
		d_t new_e {d};
		EXPECT_TRUE(new_e == e.dt);
	}
}

