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
	size_t rng_a {1103515245}; size_t rng_c {12345};  // Crude random numbers: (a*seed+c)%maxval

	size_t min_nbars = 1; size_t max_nbars = 5;
	for (int i=0; i<100; ++i) {
		size_t curr_nbars = (rng_a*i+rng_c)%(max_nbars+1);
		if (curr_nbars < min_nbars) { curr_nbars = min_nbars; }
		size_t curr_nnts = (rng_a*i+rng_c)%(12*curr_nbars+1); // 12 1/16 notes/bar
		if (curr_nnts < 3*curr_nbars) { curr_nnts = 3*curr_nbars; }

		auto rrp = randrp_metg(mg,curr_nnts,bar_t{static_cast<int>(curr_nbars)});
		
		EXPECT_TRUE(rrp.nelems()==curr_nnts);
		EXPECT_TRUE(rrp.nbars()==bar_t{static_cast<int>(curr_nbars)});
	}

}


