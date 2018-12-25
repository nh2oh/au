#include "gtest/gtest.h"
#include "..\aulib\types\nv_t.h"
#include <vector>
#include <string>
#include <cmath>
#include <numeric>

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

// 
TEST(d_t_tests, ConstructWithNegativeDurationExact) {
	d_t z {0};
	d_t dw {2}; d_t ndw {-2};
	d_t w {1}; d_t nw {-1};
	d_t h {0.5}; d_t nh {-0.5};
	d_t q {0.25}; d_t nq {-0.25};
	d_t e {0.125}; d_t ne {-0.125};
	
	EXPECT_TRUE(ndw+dw==z);
	EXPECT_TRUE(nw+w==z);
	EXPECT_TRUE(nh+h==z);
	EXPECT_TRUE(nq+q==z);
	EXPECT_TRUE(ne+e==z);

	EXPECT_TRUE(true);
}

// 
TEST(d_t_tests, CommonDurationTConstructorTests) {
	std::vector<d_t> all_cdts {
		d::ow, d::owd, d::owdd, d::owddd,
		d::qw, d::qwd, d::qwdd, d::qwddd,
		d::dw, d::dwd, d::dwdd, d::dwddd,
		d::w, d::wd, d::wdd, d::wddd,
		d::h, d::hd, d::hdd, d::hddd,
		d::q, d::qd, d::qdd, d::qddd,
		d::e, d::ed, d::edd, d::eddd,
		d::sx, d::sxd, d::sxdd, d::sxddd,
		d::t, d::td, d::tdd, d::tddd,
		d::sf, d::sfd, d::dfdd, d::sfddd,
		d::ote, d::oted, d::otedd, d::oteddd,
		d::tfs, d::tfsd, d::tfsdd, d::tfsddd,
		d::ftw, d::ftwd, d::ftwdd, d::ftwddd,
		d::ttwf, d::ttwfd, d::ttwfdd, d::ttwfddd,
		d::twfe, d::twfed, d::twfedd, d::twfeddd,
		d::fnsx, d::fnsxd, d::fnsxdd, d::fnsxddd
	};

	int i=0;
	for (int m=3; m>-13; --m) {
		for (int n=0; n<4; ++n) {
			double val = std::pow(2,m)*(2.0 - std::pow(2,-n));
			d_t nv {val};
			EXPECT_TRUE(nv == all_cdts[i]);
			EXPECT_TRUE(nv.base() == -1*m);
			EXPECT_TRUE(nv.ndot() == n);

			// For - nv_t's, base() and ndot() don't work.  
			d_t nnv {-1*val};
			d_t nnv_base = d_t {-1*std::pow(2,m)};
			EXPECT_TRUE(nv == -1*nnv);
			EXPECT_TRUE(-1*nv == nnv);
			EXPECT_TRUE(nnv == (d_t{0} - all_cdts[i]));
			d_t curr_base_nnv = nnv.base_nv();  // conserves sign, strips dots
			
			EXPECT_TRUE(curr_base_nnv == nnv_base)
				<<"i="<<i<<",m="<<m<<",n="<<n<<":  "<< curr_base_nnv.print() << " == " << nnv_base.print();

			++i;
		}
	}

	EXPECT_TRUE(d_t{d::z} == d_t{0.0});
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


// 
TEST(d_t_tests, gcd) {
	std::vector<d_t> vdt {};
	d_t cgcd {};
	d_t ans {};

	// Set 1
	vdt = {d::q,d::h,3*(d::e),d::edd,d::sx};
	cgcd = d_t{d::z};
	for (int i=0; i<vdt.size(); ++i) {
		cgcd = gcd(cgcd, vdt[i]);
	}
	EXPECT_TRUE(cgcd == d_t{d::t});

	// Set 2
	vdt = {d::q,d::h,2*(d::e),8*(d::e)};
	cgcd = d_t{d::z};
	for (int i=0; i<vdt.size(); ++i) {
		cgcd = gcd(cgcd, vdt[i]);
	}
	EXPECT_TRUE(cgcd == d_t{d::q});

	// Set 3
	vdt = {d::q,d::h,3*(d::e),d::ed,d::qd};
	cgcd = d_t{d::z};
	for (int i=0; i<vdt.size(); ++i) {
		cgcd = gcd(cgcd, vdt[i]);
	}
	EXPECT_TRUE(cgcd == d_t{d::sx});

	// Set 4
	vdt = {d::q,d::sx,d::ttwfd};
	cgcd = d_t{d::z}; ans = d_t{d::twfe};
	for (int i=0; i<vdt.size(); ++i) {
		cgcd = gcd(cgcd, vdt[i]);
	}
	EXPECT_TRUE(cgcd == ans);
	
	vdt.back() = 2*vdt.back();
	cgcd = d_t{d::z}; ans = d_t{d::ttwf};
	for (int i=0; i<vdt.size(); ++i) {
		cgcd = gcd(cgcd, vdt[i]);
	}
	EXPECT_TRUE(cgcd == ans);
}

