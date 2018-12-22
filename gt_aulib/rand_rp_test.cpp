#include "gtest/gtest.h"
#include "..\aulib\rpgen\rand_rp.h"
#include "..\aulib\types\beat_bar_t.h"
#include "..\aulib\types\nv_t.h"
#include "..\aulib\types\ts_t.h"
#include "..\aulib\types\rp_t.h"
#include <vector>
#include <chrono>


TEST(rand_rp_tests, ConstrainedNbarsConstrainedNnts) {
	randrp_input in {};
	in.ts = ts_t {4_bt,d::q};
	in.maxt = std::chrono::seconds {3};
	in.nvset = std::vector<d_t> {d::w,d::h,d::q,d::e,d::sx};
	in.pd = std::vector<double> {1,1,1,1,1};
	
	struct tests_nnts_nbars {
		int nbars {0};
		int nnts {0};
	};
	std::vector<tests_nnts_nbars> tests {
		{1,1},{1,2},{1,3},{1,4},{1,5},{1,6},{1,8},
		{2,2},{2,3},{2,4},{2,5},{2,6},{2,8},{2,9},
		{5,5},{5,8},{5,12},{5,13},{5,15},{5,19},{5,23},
	};
	
	for (const auto& e : tests) {
		in.nnts = e.nnts; in.nbars = bar_t{e.nbars};
		auto curr_rp = rand_rp(in);
		EXPECT_TRUE(curr_rp.success);
		EXPECT_EQ(curr_rp.rp.nbars(),in.nbars);
		EXPECT_EQ(curr_rp.rp.nelems(),in.nnts);
		EXPECT_EQ(curr_rp.rp.ts(),in.ts);
	}
}


TEST(rand_rp_tests, ImpossibleConstraintSets) {
	randrp_input in {};
	in.ts = ts_t {4_bt,d::q};
	in.maxt = std::chrono::seconds {1};  // Reduced timeout for these expected-to-fail tests
	in.nvset = std::vector<d_t> {d::w,d::h,d::q,d::e,d::sx};
	in.pd = std::vector<double> {1,1,1,1,1};
	
	struct tests_nnts_nbars {
		int nbars {0};
		int nnts {0};
	};
	std::vector<tests_nnts_nbars> tests {
		{2,1},{3,1},{3,2},{1,17},{1,15}
	};
	for (const auto& e : tests) {
		in.nnts = e.nnts; in.nbars = bar_t {e.nbars};
		auto curr_rp = rand_rp(in);
		EXPECT_FALSE(curr_rp.success);
	}
}


TEST(rand_rp_tests, ConstrainedNbarsUnconstrainedNnts) {
	randrp_input in {};
	in.ts = ts_t {4_bt,d::q};
	in.maxt = std::chrono::seconds {3};
	in.nvset = std::vector<d_t> {d::w,d::h,d::q,d::e,d::sx};
	in.pd = std::vector<double> {1,1,1,1,1};
	
	struct tests_nnts_nbars {
		int nbars {0};
		int nnts {0};
	};
	std::vector<tests_nnts_nbars> tests {
		{1,0},{2,0},{5,0},{10,0},{15,0},{22,0},{100,0}
	};
	
	for (const auto& e : tests) {
		in.nnts = e.nnts; in.nbars = bar_t{e.nbars};
		auto curr_rp = rand_rp(in);
		EXPECT_TRUE(curr_rp.success);
		for (int i=0; i<100; ++i ) {
			if (i==99) {
				// If rand_rp() is indeed floating nnts, it is *very* unlikely that in 99
				// tries second_rp will have the same number of notes as curr_rp.  
				EXPECT_TRUE(false);
			}
			auto second_rp = rand_rp(in);
			EXPECT_TRUE(second_rp.success);
			if (second_rp.rp.nelems()!=curr_rp.rp.nelems()) {
				break;
			}
		}
		
		EXPECT_EQ(curr_rp.rp.nbars(),in.nbars);
		EXPECT_EQ(curr_rp.rp.ts(),in.ts);
	}
}



TEST(rand_rp_tests, UnconstrainedNbarsConstrainedNnts) {
	randrp_input in {};
	in.ts = ts_t {4_bt,d::q};
	in.maxt = std::chrono::seconds {3};
	in.nvset = std::vector<d_t> {d::w,d::h,d::q,d::e,d::sx};
	in.pd = std::vector<double> {1,1,1,1,1};
	
	struct tests_nnts_nbars {
		int nbars {0};
		int nnts {0};
	};
	std::vector<tests_nnts_nbars> tests {
		{0,2},{0,3},{0,4},{0,13},{0,27},{0,100}
	};
	// TODO:
	// Bug:  {0,1} fails since rpseg must always be exactly == 1 bar
	
	for (const auto& e : tests) {
		in.nnts = e.nnts; in.nbars = bar_t{e.nbars};
		auto curr_rp = rand_rp(in);
		EXPECT_TRUE(curr_rp.success);
		for (int i=0; i<100; ++i ) {
			if (i==99) {
				// If rand_rp() is indeed floating nnts, it is *very* unlikely that in 99
				// tries second_rp will have the same number of notes as curr_rp.  
				EXPECT_TRUE(false);
			}
			auto second_rp = rand_rp(in);
			EXPECT_TRUE(second_rp.success);
			if (second_rp.rp.nbars()!=curr_rp.rp.nbars()) {
				break;
			}
		}
		
		EXPECT_EQ(curr_rp.rp.nelems(),in.nnts);
		EXPECT_EQ(curr_rp.rp.ts(),in.ts);
	}
}

