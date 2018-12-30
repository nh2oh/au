#include "gtest/gtest.h"
#include "..\aulib\types\rp_t.h"
#include "..\aulib\types\ts_t.h"
#include "..\aulib\types\beat_bar_t.h"
#include <vector>


// For a set of rp's of known nbars, nbeats under three ts_t's == 4/4, 3/4, 5/4,
// tests rp2_t.nbeat(), .nbar(), .nevents() for construction from a vector<d_t>.
TEST(rp2_t_tests, VectorDtConstructorAssortedRPsAndTsSet1) {
	std::vector<std::vector<d_t>> rp{
		{d::q, d::q, d::h},  //  4/4 => 1 bar
		{d::e, d::e, d::q, d::h},  // 4/4 => 1 bar
		{d::w, d::h, d::q, d::q},  // 4/4 => 2 bars
		{d::h, d::q, d::q, d::w, d::e, d::e, d::e, d::e, d::e, d::e, d::e, d::e},  // 4/4 => 3 bars
		{d::e, d::e, d::q, d::w, d::h, d::h, d::e, d::e, d::q, d::q, d::q, d::q, d::q}  // 4/4 => 4 bars
	};

	std::vector<ts_t> ts {
		ts_t{4_bt, d::q, false},
		ts_t{2_bt, d::q, false},
		ts_t{5_bt, d::q, false}
	};

	std::vector<std::vector<beat_t>> ans_nbeat {
		{4_bt, 4_bt, 8_bt, 12_bt, 16_bt},  // 4/4
		{4_bt, 4_bt, 8_bt, 12_bt, 16_bt},  // 2/4
		{4_bt, 4_bt, 8_bt, 12_bt, 16_bt}   // 5/4
	};
	std::vector<std::vector<bar_t>> ans_nbar {
		{1_br, 1_br, 2_br, 3_br, 4_br},  // 4/4
		{2_br, 2_br, 4_br, 6_br, 8_br},  // 2/4
		{0.8_br, 0.8_br, 1.6_br, 2.4_br, 3.2_br}  // 5/4
	};

	for (int i=0; i<ts.size(); ++i) {
		auto curr_ts = ts[i];
		for (int j=0; j<rp.size(); ++j) {
			auto curr_vdt = rp[j];

			rp2_t curr_rp {curr_ts, curr_vdt};

			EXPECT_TRUE(curr_rp.ts() == curr_ts);
			EXPECT_TRUE(curr_rp.nbeats() == ans_nbeat[i][j]) << "Expect " 
				<< curr_rp.nbeats().print() << " == " << ans_nbeat[i][j].print();
			EXPECT_TRUE(curr_rp.nbars() == ans_nbar[i][j]) << "Expect " 
				<< curr_rp.nbars().print() << " == " << ans_nbar[i][j].print();
			EXPECT_TRUE(curr_rp.nevents() == curr_vdt.size()) << "Expect " 
				<< curr_rp.nevents() << " == " << curr_vdt.size();

			EXPECT_TRUE(curr_rp.to_duration_seq() == curr_vdt);

			for (int k=0; k<curr_vdt.size(); ++k) {
				EXPECT_TRUE(curr_rp[k].e==curr_vdt[k]);
			}
		}  // to next rp for curr_ts
	} // to next ts
}


// For a set of rp's of known nbars, nbeats under three ts_t's == 4/4, 3/4, 5/4,
// tests rp2_t.nbeat(), .nbar(), .nevents() for construction empty, followed by 
// element-wise push_back() of each d_t element into the rp2_t.
TEST(rp2_t_tests, TsOnlyConstructorAssortedRPsAndTsSet1) {
	std::vector<std::vector<d_t>> rp{
		{d::q, d::q, d::h},  //  4/4 => 1 bar
		{d::e, d::e, d::q, d::h},  // 4/4 => 1 bar
		{d::w, d::h, d::q, d::q},  // 4/4 => 2 bars
		{d::h, d::q, d::q, d::w, d::e, d::e, d::e, d::e, d::e, d::e, d::e, d::e},  // 4/4 => 3 bars
		{d::e, d::e, d::q, d::w, d::h, d::h, d::e, d::e, d::q, d::q, d::q, d::q, d::q}  // 4/4 => 4 bars
	};

	std::vector<ts_t> ts{
		ts_t{4_bt, d::q, false},
		ts_t{2_bt, d::q, false},
		ts_t{5_bt, d::q, false}
	};

	std::vector<std::vector<beat_t>> ans_nbeat{
		{4_bt, 4_bt, 8_bt, 12_bt, 16_bt},  // 4/4
		{4_bt, 4_bt, 8_bt, 12_bt, 16_bt},  // 2/4
		{4_bt, 4_bt, 8_bt, 12_bt, 16_bt}   // 5/4
	};
	std::vector<std::vector<bar_t>> ans_nbar{
		{1_br, 1_br, 2_br, 3_br, 4_br},  // 4/4
		{2_br, 2_br, 4_br, 6_br, 8_br},  // 2/4
		{0.8_br, 0.8_br, 1.6_br, 2.4_br, 3.2_br}  // 5/4
	};

	for (int i=0; i<ts.size(); ++i) {
		auto curr_ts = ts[i];
		for (int j=0; j<rp.size(); ++j) {
			auto curr_vdt = rp[j];

			rp2_t curr_rp{curr_ts};
			EXPECT_TRUE(curr_rp.ts() == curr_ts);
			EXPECT_TRUE(curr_rp.nbeats() == 0_bt);
			EXPECT_TRUE(curr_rp.nbars() == 0_br);
			EXPECT_TRUE(curr_rp.nevents() == 0);

			for (int k=0; k<curr_vdt.size(); ++k) {
				curr_rp.push_back(curr_vdt[k]);
			}
			EXPECT_TRUE(curr_rp.ts() == curr_ts);
			EXPECT_TRUE(curr_rp.nbeats() == ans_nbeat[i][j]);
			EXPECT_TRUE(curr_rp.nbars() == ans_nbar[i][j]);
			EXPECT_TRUE(curr_rp.nevents() == curr_vdt.size());

			EXPECT_TRUE(curr_rp.to_duration_seq() == curr_vdt);

			for (int k=0; k<curr_vdt.size(); ++k) {
				EXPECT_TRUE(curr_rp[k].e==curr_vdt[k]);
			}
		}  // to next rp for curr_ts
	} // to next ts
}



TEST(rp2_t_tests, VectorDtConstructorAssortedRPsAndTsSet2) {
	std::vector<std::vector<d_t>> rp{
		{d::sx, d::sx, d::sx, d::sx, d::sx, d::sx, d::sx, d::sx, d::sx, d::sx, d::sx, d::sx}, // 12 sx => 1 bar
		{d::sx, d::sx, d::sx, d::sx, d::sx, d::sx, d::sx, d::sx}, // 8 sx => 2/3 bars 
		{d::ote, d::sx, d::sx, d::sx, d::sx, d::sx, d::sx, d::sx, d::sx, d::sx, d::sx, d::sx, d::sx}
			// 12(1/16)+1/128 => 1+1/96'th bar
	};

	std::vector<ts_t> ts {
		ts_t{3_bt, d::q, false},
	};

	std::vector<std::vector<beat_t>> ans_nbeat {
		{3_bt, 2_bt, 3_bt+0.03125_bt} // There are 32 1/128'th notes in a 1/4 note; 1/32 == 0.03125
	};
	std::vector<std::vector<bar_t>> ans_nbar {
		{1_br, bar_t{2.0/3.0}, 1_br + bar_t{1.0/96.0}}  // 3 1/4 notes per bar => 3x32==96 1/128 notes
	};

	for (int i=0; i<ts.size(); ++i) {
		auto curr_ts = ts[i];
		for (int j=0; j<rp.size(); ++j) {
			auto curr_vdt = rp[j];

			rp2_t curr_rp{curr_ts, curr_vdt};
			EXPECT_TRUE(curr_rp.ts() == curr_ts);
			EXPECT_TRUE(curr_rp.nbeats() == ans_nbeat[i][j]);
			EXPECT_TRUE(curr_rp.nbars() == ans_nbar[i][j]);
			EXPECT_TRUE(curr_rp.nevents() == curr_vdt.size());

			EXPECT_TRUE(curr_rp.to_duration_seq() == curr_vdt);

			for (int k=0; k<curr_vdt.size(); ++k) {
				EXPECT_TRUE(curr_rp[k].e==curr_vdt[k]);
			}
		}  // to next rp for curr_ts
	} // to next ts
}

