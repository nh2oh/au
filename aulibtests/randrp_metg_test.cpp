#include "pch.h"
#include "..\aulib\types\beat_bar_t.h"
#include "..\aulib\types\nv_t.h"
#include "..\aulib\types\ts_t.h"
#include "..\aulib\types\metg_t.h"
#include "..\aulib\types\rp_t.h"
#include "..\aulib\randrp_metg.h"
#include <vector>
#include <numeric>
#include <limits>


// A simple mg in 4/4
TEST(randrp_metg_tests, FourFourZeroPhaseQESxDefaultRandommg) {
	ts_t ts {4_bt,d::q};
	tmetg_t mg {ts,{d::q,d::e,d::sx},{0_bt,0_bt,0_bt}};
	size_t rng_a {1103515245}; size_t rng_c {12345};  // Crude random numbers: (a*seed+c)%maxval

	size_t min_nbars = 1; size_t max_nbars = 5;
	for (int i=0; i<100; ++i) {
		size_t curr_nbars = (rng_a*i+rng_c)%(max_nbars+1);
		if (curr_nbars < min_nbars) { curr_nbars = min_nbars; }
		size_t curr_nnts = (rng_a*i+rng_c)%(16*curr_nbars+1);
		if (curr_nnts < 4*curr_nbars) { curr_nnts = 4*curr_nbars; }

		auto rrp = randrp_metg(mg,curr_nnts,bar_t{static_cast<int>(curr_nbars)});
		
		EXPECT_TRUE(rrp.nelems()==curr_nnts);
		EXPECT_TRUE(rrp.nbars()==bar_t{static_cast<int>(curr_nbars)});
	}

}


// A simple mg in 3/4
TEST(randrp_metg_tests, ThreeFourZeroPhaseHQESxDefaultRandommg) {
	ts_t ts {3_bt,d::q};
	tmetg_t mg {ts,{d::h,d::q,d::e,d::sx},{0_bt,0_bt,0_bt,0_bt}};

	struct test_set {
		int num_nts {0};
		bar_t num_bars {0};
	};
	// Just a bunch of "random" tests:
	// num_bars varies on [1,5]; num_nts varies on [3*nbars,12*nbars] (this mg 
	// has 12 1/16 notes/bar).  
	// {nnts, nbars} pairs generated w/:
	//   size_t rng_a {1103515245}; size_t rng_c {12345};  // => (a*seed+c)%maxval
	//   size_t curr_nbars = (rng_a*i+rng_c)%(max_nbars+1);
	//   if (curr_nbars < min_nbars) { curr_nbars = min_nbars; }
	//   size_t curr_nnts = (rng_a*i+rng_c)%(12*curr_nbars+1);
	//   if (curr_nnts < 3*curr_nbars) { curr_nnts = 3*curr_nbars;
	//
	std::vector<test_set> tests {
		{24,3_br},{9,1_br},{13,3_br},{11,1_br},{9,3_br},{3,1_br},{28,3_br},{3,1_br},
		{17,3_br},{4,1_br},{9,3_br},{6,1_br},{32,3_br},{8,1_br},{21,3_br},{10,1_br},
		{10,3_br},{12,1_br},{36,3_br},{3,1_br},{25,3_br},{3,1_br},{14,3_br},{5,1_br},
		{9,3_br},{7,1_br},{29,3_br},{9,1_br},{18,3_br},{11,1_br},{9,3_br},{3,1_br},
		{33,3_br},{3,1_br},{22,3_br},{4,1_br},{11,3_br},{6,1_br},{9,3_br},{8,1_br},
		{26,3_br},{10,1_br},{15,3_br},{12,1_br},{9,3_br},{3,1_br},{30,3_br},{3,1_br},
		{19,3_br},{5,1_br},{9,3_br},{7,1_br},{34,3_br},{9,1_br},{23,3_br},{11,1_br},
		{12,3_br},{3,1_br},{9,3_br},{3,1_br},{27,3_br},{4,1_br},{16,3_br},{6,1_br},
		{9,3_br},{8,1_br},{31,3_br},{10,1_br},{20,3_br},{12,1_br},{9,3_br},{3,1_br},
		{35,3_br},{3,1_br},{24,3_br},{5,1_br},{13,3_br},{7,1_br},{9,3_br},{9,1_br},
		{28,3_br},{11,1_br},{17,3_br},{3,1_br},{9,3_br},{3,1_br},{32,3_br},{4,1_br},
		{21,3_br},{6,1_br},{10,3_br},{8,1_br},{36,3_br},{10,1_br},{25,3_br},{12,1_br},
		{14,3_br},{3,1_br},{9,3_br},{3,1_br}
	};

	for (int i=0; i<tests.size(); ++i) {
		auto rrp = randrp_metg(mg,tests[i].num_nts,tests[i].num_bars);
		
		EXPECT_TRUE(rrp.nelems()==tests[i].num_nts);
		auto nb = rrp.nbars();
		auto tf_nb = (nb==tests[i].num_bars);
		EXPECT_TRUE(tf_nb);
	}

}


