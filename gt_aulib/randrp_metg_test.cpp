#include "gtest/gtest.h"
#include "dbklib\math.h"  // transpose()
#include "..\aulib\types\beat_bar_t.h"
#include "..\aulib\types\nv_t.h"
#include "..\aulib\types\ts_t.h"
#include "..\aulib\types\metg_t.h"
#include "..\aulib\types\rp_t.h"
#include "..\aulib\randrp_metg.h"
#include <vector>
#include <numeric>


// A simple mg in 4/4; all phases zero
// Constraining nnts and nbars
TEST(randrp_metg_tests, FourFourQESxZeroPhDefMgConstrainNbarsNnts) {
	ts_t ts {4_bt,d::q};
	tmetg_t mg {ts,{d::q,d::e,d::sx},{0_bt,0_bt,0_bt}};

	struct test_set {
		int num_nts {0};
		bar_t num_bars {0};
	};
	// Just a bunch of "random" tests:
	// num_bars varies on [1,5]; num_nts varies on [4*nbars,16*nbars] (this mg 
	// has 16 1/16 notes/bar).  
	//
	std::vector<test_set> tests {
		{5,1_br},{9,1_br},{10,1_br},{13,1_br},{16,1_br},{12,2_br},{13,2_br},
		{17,2_br},{21,2_br},{22,2_br},{29,2_br},{30,2_br},{18,3_br},{22,3_br},
		{26,3_br},{30,3_br},{42,3_br},{43,3_br},{48,3_br},{23,4_br},{28,4_br},
		{29,4_br},{41,4_br},{46,4_br},{54,4_br},{61,4_br},{63,4_br},{22,5_br},
		{42,5_br},{46,5_br},{63,5_br},{65,5_br},{79,5_br}
	};

	for (int i=0; i<tests.size(); ++i) {
		auto rrp = randrp_metg(mg,tests[i].num_nts,tests[i].num_bars);
		
		EXPECT_TRUE(rrp.nevents()==tests[i].num_nts);
		auto nb = rrp.nbars();
		auto tf_nb = (nb==tests[i].num_bars);
		EXPECT_TRUE(tf_nb);
	}
}

// A simple mg in 4/4; all phases zero; q,e,sx
// Constraining nnts only
TEST(randrp_metg_tests, FourFourQESxZeroPhDefMgConstrainNnotes) {
	ts_t ts {4_bt,d::q};
	tmetg_t mg {ts,{d::q,d::e,d::sx},{0_bt,0_bt,0_bt}};

	struct test_set {
		int num_nts {0};
		bar_t num_bars {0};
	};

	// tests.size() == 24
	std::vector<test_set> tests {};
	for (int i=1; i<50; i+=2) {  // NB:  Starts @ 1
		tests.push_back({3*i,0_br});
	}

	for (int i=0; i<tests.size(); ++i) {
		auto rrp = randrp_metg(mg,tests[i].num_nts,tests[i].num_bars);
		EXPECT_TRUE(rrp.nevents()==tests[i].num_nts);
		EXPECT_TRUE(rrp.nbars() >= rrp.nevents()*nbar(mg.ts(),mg.levels().back().nv));
		EXPECT_TRUE(rrp.nbars() <= rrp.nevents()*nbar(mg.ts(),mg.levels().front().nv));
	}
}


// A simple mg in 4/4; all phases zero; q,e,sx
// Constraining nbars only; noninteger nbars
TEST(randrp_metg_tests, FourFourQESxZeroPhDefMgConstrainNbars) {
	ts_t ts {4_bt,d::q};
	tmetg_t mg {ts,{d::q,d::e,d::sx},{0_bt,0_bt,0_bt}};

	struct test_set {
		int num_nts {0};
		bar_t num_bars {0};
	};
	// tests.size() == 24; d::sx => 0.0625 bars, 49*d::sx => 3.0625 bars
	std::vector<test_set> tests {};
	for (int i=1; i<50; i+=2) {  // NB:  Starts @ 1
		bar_t curr_nbars {i*nbar(mg.ts(),mg.levels().back().nv)};
		tests.push_back({0,curr_nbars});
	}

	for (int i=0; i<tests.size(); ++i) {
		auto rrp = randrp_metg(mg,tests[i].num_nts,tests[i].num_bars);
		
		EXPECT_TRUE(rrp.nevents() > 0);
		auto nb = rrp.nbars();
		auto tf_nb = (nb==tests[i].num_bars);
		EXPECT_TRUE(tf_nb);
	}
}


// A simple mg in 4/4; all phases zero; dw,w,q,e (the dw note always spans a bar)
// Constraining nbars only; noninteger nbars
TEST(randrp_metg_tests, FourFourDwWQEZeroPhDefMgConstrainNbars) {
	ts_t ts {4_bt,d::q};
	tmetg_t mg {ts,{d::dw,d::w,d::q,d::e},{0_bt,0_bt,0_bt,0_bt}};

	struct test_set {
		int num_nts {0};
		bar_t num_bars {0};
	};

	// tests.size() == 24; d::e => 0.125 bars, *d::e => 12.125 bars
	std::vector<test_set> tests {};
	for (int i=1; i<100; i+=4) {  // NB:  Starts @ 1
		bar_t curr_nbars {i*nbar(mg.ts(),mg.levels().back().nv)};
		tests.push_back({0,curr_nbars});
	}

	for (int i=0; i<tests.size(); ++i) {
		auto rrp = randrp_metg(mg,tests[i].num_nts,tests[i].num_bars);
		
		EXPECT_TRUE(rrp.nevents() > 0);
		auto nb = rrp.nbars();
		auto tf_nb = (nb==tests[i].num_bars);
		EXPECT_TRUE(tf_nb);
	}
}


// A simple mg in 4/4; all phases zero; w,h,q; pg manually set to only allow
// a specific, small set of rps
// Constraining nbars only, nnts only, and both
TEST(randrp_metg_tests, FourFourWHQZeroPhManualPg) {
	struct test_set {
		int num_nts {0};
		bar_t num_bars {0};
	};
	ts_t ts {4_bt,d::q};
	teejee tg {ts,{d::w,d::h,d::q},{0_bt,0_bt,0_bt}};

	std::vector<std::vector<double>> pg {
		{1,0,0,0},
		{1,0,1,0},
		{1,1,0,0}
	};
	// 1 bar:  w; h,h; q,q,h
	// 1.5 bars:  w,h; w,q,q; h,h,h; h,h,q,q; q,q,h,h; q,q,h,q,q
	//auto pgt = transpose_randrpmetg_gtest(pg);
	auto pgt = transpose(pg);
	tmetg_t mg {ts,tg.levels(),pgt};
	auto tfval = mg.validate();  EXPECT_TRUE(tfval);

	// 1 bar exactly
	std::vector<test_set> tests1 {{1,1_br},{2,1_br},{3,1_br}};
	std::vector<std::vector<d_t>> rp_ans_set1 {{d::w},{d::h,d::h},{d::q,d::q,d::h}};
	int Ntests1 = 10*tests1.size();
	for (int i=0; i<Ntests1; ++i) {
		auto rrp = randrp_metg(mg,tests1[i%tests1.size()].num_nts,tests1[i%tests1.size()].num_bars);

		EXPECT_TRUE(rrp.nevents()==tests1[i%tests1.size()].num_nts);
		auto nb = rrp.nbars();
		auto tf_nb = (nb==tests1[i%tests1.size()].num_bars);
		EXPECT_TRUE(tf_nb);

		rp_t curr_rp_ans {mg.ts(),rp_ans_set1[i%tests1.size()]};
		bool tf_rp_ans = curr_rp_ans.to_duration_seq() == rrp.to_duration_seq();
		EXPECT_TRUE(tf_rp_ans);
	}
	
	// 1.5 bars exactly
	std::vector<test_set> tests2 {{2,1.5_br},{3,1.5_br},{4,1.5_br}};
	std::vector<std::vector<d_t>> rp_ans_set {{d::w,d::h},{d::w,d::q,d::q},
		{d::h,d::h,d::h}, {d::h,d::h,d::q,d::q}, {d::q,d::q,d::h,d::h},
		{d::q,d::q,d::h,d::q,d::q}};
	int Ntests2 = 10*tests2.size();
	for (int i=0; i<Ntests2; ++i) {
		auto rrp = randrp_metg(mg,tests2[i%tests2.size()].num_nts,tests2[i%tests2.size()].num_bars);

		EXPECT_TRUE(rrp.nevents()==tests2[i%tests2.size()].num_nts);
		auto nb = rrp.nbars();
		auto tf_nb = (nb==tests2[i%tests2.size()].num_bars);
		EXPECT_TRUE(tf_nb);

		bool found_rp = false;
		for (auto e : rp_ans_set) {
			if (e == rrp.to_duration_seq()) {
				found_rp = true;
				break;
			}
		}
		EXPECT_TRUE(found_rp);
	}
}


// A simple mg in 4/4; w,h,q; the w-note has a 1-bt phase shift.  
// This mg does not factor!  The w and h notes always cross over oneanother
TEST(randrp_metg_tests, FourFourHnt1BtPhShiftWHQDefaultRandommg) {
	ts_t ts {4_bt,d::q};
	tmetg_t mg {ts,{d::w,d::h,d::q},{1_bt,0_bt,0_bt}};

	struct test_set {
		int num_nts {0};
		bar_t num_bars {0};
	};
	// Just a bunch of "random" tests:
	// num_bars varies on [1,6]; num_nts varies on [2*nbars,4*nbars]
	std::vector<test_set> tests {
		{3,1_br},{4,1_br},{4,2_br},{5,2_br},{7,2_br},{8,2_br},{10,3_br},{9,4_br},
		{10,4_br},{13,4_br},{15,5_br},{17,5_br},{18,5_br},{19,5_br},{20,5_br},
		{16,6_br},{19,6_br},{22,6_br}
	};

	for (int i=0; i<tests.size(); ++i) {
		auto rrp = randrp_metg(mg,tests[i].num_nts,tests[i].num_bars);
		bool tf_ne = (rrp.nevents()==tests[i].num_nts);
		EXPECT_TRUE(tf_ne);
		auto nb = rrp.nbars();
		auto tf_nb = (nb==tests[i].num_bars);
		EXPECT_TRUE(tf_nb);
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
	std::vector<test_set> tests {
		{20,5_br},{3,1_br},{4,1_br},{6,1_br},{9,1_br},{10,1_br},{11,1_br},{12,2_br},
		{13,2_br},{17,2_br},{18,2_br},{20,2_br},{10,3_br},{11,3_br},{13,3_br},{16,3_br},
		{20,3_br},{21,3_br},{26,4_br},{27,4_br},{36,4_br},{38,4_br},{40,4_br},{42,4_br},
		{48,4_br},{20,5_br},{21,5_br},{28,5_br},{29,5_br},{32,5_br},{37,5_br},{41,5_br}
	};

	for (int i=0; i<tests.size(); ++i) {
		auto rrp = randrp_metg(mg,tests[i].num_nts,tests[i].num_bars);
		
		EXPECT_TRUE(rrp.nevents()==tests[i].num_nts);
		auto nb = rrp.nbars();
		auto tf_nb = (nb==tests[i].num_bars);
		EXPECT_TRUE(tf_nb);
	}

}


