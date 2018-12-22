#include "gtest/gtest.h"
#include "..\aulib\types\ts_t.h"
#include "..\aulib\types\beat_bar_t.h"
#include "..\aulib\util\au_algs_math.h"
#include <vector>

//
// TODO:  Currently there are no tests for compound signatures.  
// TODO:  See the ts_t {_bt,d_t,bool} ctor.  For compound signatures, std::abort()
//        is called if d_t.add_dots(1) returns false, which currently is the case if
//        d_t.singlet_exists() == false; do i want this?  
// TODO:  Add tests for string ctor w/ dotted note durations
//

// Default ts is 4/4 simple
TEST(ts_t_tests, DefaultConstructor) {
	ts_t def_ts {};
	ts_t fourfour {4_bt,d::q};
	EXPECT_TRUE(def_ts == fourfour);
}

// Tests beats_per_bar(), beat_unit(), bar_unit() with known values
// for known inputs
TEST(ts_t_tests, SixEightSimpleCompound) {
	ts_t ses {6_bt,d::e,false};
	ts_t sec {6_bt,d::e,true};
	EXPECT_TRUE(ses != sec);

	EXPECT_TRUE(ses.beats_per_bar() == 6_bt);
	EXPECT_TRUE(sec.beats_per_bar() == 2_bt);

	EXPECT_TRUE(ses.beat_unit() == d_t{d::e});
	EXPECT_TRUE(sec.beat_unit() == d_t{d::qd});

	EXPECT_TRUE(ses.bar_unit() == d_t{d::hd});
	EXPECT_TRUE(sec.bar_unit() == d_t{d::hd});
}

// Tests beats_per_bar(), beat_unit(), bar_unit() with known values
// for known inputs
TEST(ts_t_tests, TwelveEightSimpleCompound) {
	ts_t tes {12_bt,d::e,false};
	ts_t tec {12_bt,d::e,true};
	EXPECT_TRUE(tes != tec);

	EXPECT_TRUE(tes.beats_per_bar() == 12_bt);
	EXPECT_TRUE(tec.beats_per_bar() == 4_bt);

	EXPECT_TRUE(tes.beat_unit() == d_t{d::e});
	EXPECT_TRUE(tec.beat_unit() == d_t{d::qd});

	EXPECT_TRUE(tes.bar_unit() == d_t{d::wd});
	EXPECT_TRUE(tec.bar_unit() == d_t{d::wd});
}


// Tests the ctor from a std::string for non-compound ts_t's
TEST(ts_t_tests, StringCtorNotCompound) {
	struct ts_str_ans {
		std::string str {};
		ts_t ans {};
	};
	
	std::vector<ts_str_ans> tests {
		{"4/4", ts_t {4_bt,d::q,false}}, {"04/04", ts_t {4_bt,d::q,false}}, 
		{"0004/00004", ts_t {4_bt,d::q,false}},
		{"6/8", ts_t {6_bt,d::e,false}}, {"06/08", ts_t {6_bt,d::e,false}},
		{"0006/00008", ts_t {6_bt,d::e,false}},
		{"12/16", ts_t {12_bt,d::sx,false}},{"012/016", ts_t {12_bt,d::sx,false}},
		{"00012/000016", ts_t {12_bt,d::sx,false}},
	};
	
	for (const auto& e : tests) {
		EXPECT_EQ(ts_t {e.str}, e.ans);
	}

}

// Tests ts_t.print(); the output of which, if fed into ts_t(std::string), should produce
// the same ts.  
TEST(ts_t_tests, PrintFnAndStringCtor) {
	struct ts_str_ans {
		std::string str {};
		ts_t ans {};
	};
	
	std::vector<ts_str_ans> tests {
		{"4/4", ts_t {4_bt,d::q,false}}, {"04/04", ts_t {4_bt,d::q,false}}, 
		{"4/4c", ts_t {4_bt,d::q,true}}, {"04/04c", ts_t {4_bt,d::q,true}},
		{"6/8", ts_t {6_bt,d::e,false}}, {"06/08c", ts_t {6_bt,d::e,true}},
		{"0006/00008", ts_t {6_bt,d::e,false}},
		{"12/16", ts_t {12_bt,d::sx,false}},{"012/016", ts_t {12_bt,d::sx,true}},
		{"00012/000016", ts_t {12_bt,d::sx,false}},
	};

	for (const auto& e : tests) {
		ts_t from_print {e.ans.print()};
		EXPECT_EQ(e.ans, from_print);
		EXPECT_EQ(e.ans.print(), from_print.print());
	}
}

// Tests ts_t.print(); the output of which, if fed into ts_t(std::string), should produce
// the same ts.  
TEST(ts_t_tests, ParseTsString) {
	struct pts_tests {  // "parse-ts-string tests"
		std::string str {};
		ts_str_parsed ans {};
	};
	
	std::vector<pts_tests> tests {
		{"4/4", {true,false,4,4}}, {"04/04", {true,false,4,4}}, 
		{"4/4c", {true,true,4,4}}, {"04/04c", {true,true,4,4}},
		{"6/8", {true,false,6,8}}, {"06/08c", {true,true,6,8}},
		{"0006/00008", {true,false,6,8}},
		{"12/16", {true,false,12,16}},{"012/016", {true,false,12,16}},
		{"00012/000016", {true,false,12,16}},
	};

	for (const auto& e : tests) {
		EXPECT_TRUE(e.ans.is_valid);
		ts_t curr_ts {beat_t{e.ans.num},d_t{e.ans.denom},e.ans.is_compound};

		if (!e.ans.is_compound) {
			EXPECT_EQ(beat_t{e.ans.num}, curr_ts.beats_per_bar()) 
				<< e.str << " : " << beat_t{e.ans.num}.print() << ", " 
				<< curr_ts.beats_per_bar().print();

			EXPECT_EQ(d_t{(e.ans.denom)*(e.ans.num)}, curr_ts.bar_unit())
				<< e.str << " : " << d_t{e.ans.denom}.print() << ", " 
				<< curr_ts.bar_unit().print();

			EXPECT_EQ(d_t{e.ans.denom}, curr_ts.beat_unit()) << e.str;
		} else {  // compound
			EXPECT_EQ(beat_t{(1.0/3.0)*e.ans.num}, curr_ts.beats_per_bar())
				<< e.str << " : " << beat_t{e.ans.num}.print() << ", " 
				<< curr_ts.beats_per_bar().print();

				EXPECT_EQ(d_t{(e.ans.denom)*(e.ans.num)}, curr_ts.bar_unit())
				<< e.str << " : " << d_t{e.ans.denom}.print() << ", " 
				<< curr_ts.bar_unit().print();

			EXPECT_EQ(d_t{2*e.ans.denom + e.ans.denom}, curr_ts.beat_unit());
			
		}
	}
}


// Tests nbeat(ts_t,d_t), duration(ts_t,beat_t), nbar(ts_t,d_t) with different 
// d_t, ts_t values known to yield the same output.  
// ts = 4/4, 3/4
TEST(ts_t_tests, EquivDurationsThreeFourFourFour) {
	ts_t tf {3_bt,d::q,false};
	ts_t ff {4_bt,d::q,false};

	{
		std::vector<d_t> vdt_ff {d::dw,d::w,d::h,d::q,d::e,d::sx,d::t};
		std::vector<d_t> vdt_tf {d::dw,d::w,d::h,d::q,d::e,d::sx,d::t};
		for (int i=0; i<vdt_ff.size(); ++i) {
			EXPECT_TRUE(nbeat(tf,vdt_tf[i]) == nbeat(ff,vdt_ff[i]));
			EXPECT_TRUE(duration(tf,nbeat(tf,vdt_tf[i])) == 
				duration(ff,nbeat(ff,vdt_ff[i])));
		}
	}

	{
		std::vector<d_t> vdt_ff {d::dw,d::w, d::h, d::q, d::e,  d::sx,d::t};
		std::vector<d_t> vdt_tf {d::wd,d::hd,d::qd,d::ed,d::sxd,d::td,d::sfd};
		for (int i=0; i<vdt_ff.size(); ++i) {
			EXPECT_TRUE(nbar(tf,vdt_tf[i]) == nbar(ff,vdt_ff[i]));
		}
	}

}

// Tests bar_t.isexact(), bar_t.fremain(), bar_t.full(), bar_t.next() for 
// accumulation into a bar_t variable using ksum<bar_t>.  
// Each iteration adds a d::q to the sum.  
TEST(ts_t_tests, ThreeFourIterativeNbarCalculations) {
	ts_t ts {3_bt,d::q,false};

	ksum<bar_t> cum_nbars {};
	for (int i=1; i<100; ++i) {  // Note: starting at 1
		cum_nbars += nbar(ts, d_t{d::q});

		if (i%3==0) {
			bool isex = cum_nbars.value.isexact(); EXPECT_TRUE(isex);
			double frem = cum_nbars.value.fremain(); EXPECT_EQ(frem,1.0);
			bar_t nfull = cum_nbars.value.full(); EXPECT_EQ(nfull,(i/3)*1_br);
			bar_t nnext = cum_nbars.value.next(); EXPECT_EQ(nnext,(i/3)*1_br+1_br);
			EXPECT_EQ(cum_nbars.value,(i/3)*(1_br));
		}

	}

}


// For a set of rp's of known nbars, nbeats under three ts_t's == 4/4, 3/4, 5/4,
// tests nbeat() and nbar() and the ability to repeatedly add into a beat_t, and
// bar_t variable (does not use ksum).  
TEST(ts_t_tests, FourTwoFiveFourBeatCalcsForAssortedRPs) {
	std::vector<std::vector<d_t>> rp {
		{d::q,d::q,d::h},  //  4/4 => 1 bar
		{d::e,d::e,d::q,d::h},  // 4/4 => 1 bar
		{d::w,d::h,d::q,d::q},  // 4/4 => 2 bars
		{d::h,d::q,d::q,d::w,d::e,d::e,d::e,d::e,d::e,d::e,d::e,d::e},  // 4/4 => 3 bars
		{d::e,d::e,d::q,d::w,d::h,d::h,d::e,d::e,d::q,d::q,d::q,d::q,d::q}  // 4/4 => 4 bars
	};

	std::vector<ts_t> ts {
		ts_t {4_bt,d::q,false},
		ts_t {2_bt,d::q,false},
		ts_t {5_bt,d::q,false}
	};

	std::vector<std::vector<beat_t>> ans_nbeat {
		{4_bt,4_bt,8_bt,12_bt,16_bt},  // 4/4
		{4_bt,4_bt,8_bt,12_bt,16_bt},  // 2/4
		{4_bt,4_bt,8_bt,12_bt,16_bt}   // 5/4
	};
	std::vector<std::vector<bar_t>> ans_nbar {
		{1_br,1_br,2_br,3_br,4_br},  // 4/4
		{2_br,2_br,4_br,6_br,8_br},  // 2/4
		{0.8_br,0.8_br,1.6_br,2.4_br,3.2_br}  // 5/4
	};

	for (int i=0; i<ts.size(); ++i) {
		auto curr_ts = ts[i];
		for (int j=0; j<rp.size(); ++j) {
			auto curr_rp = rp[j];

			beat_t cum_nbeats {0};
			bar_t cum_nbars {0};
			for (int k=0; k<curr_rp.size(); ++k) {
				cum_nbeats += nbeat(curr_ts,curr_rp[k]);
				cum_nbars += nbar(curr_ts,curr_rp[k]);
			}
			EXPECT_TRUE(cum_nbeats == ans_nbeat[i][j]);
			EXPECT_TRUE(cum_nbars == ans_nbar[i][j]);

			EXPECT_TRUE(nbeat(curr_ts,cum_nbars) == cum_nbeats);
			EXPECT_TRUE(nbar(curr_ts,cum_nbeats) == cum_nbars);
		}
	}
}


TEST(ts_t_tests, ThreeFourBeatCalcsForAssortedRPs) {
	std::vector<std::vector<d_t>> rp {
		{d::sx,d::sx,d::sx,d::sx,d::sx,d::sx,d::sx,d::sx,d::sx,d::sx,d::sx,d::sx}, // 12 sx => 1 bar
		{d::sx,d::sx,d::sx,d::sx,d::sx,d::sx,d::sx,d::sx}, // 8 sx => 2/3 bars 
		{d::ote,d::sx,d::sx,d::sx,d::sx,d::sx,d::sx,d::sx,d::sx,d::sx,d::sx,d::sx,d::sx}
			// 12(1/16)+1/128 => 1+1/96'th bar
	};

	std::vector<ts_t> ts {
		ts_t {3_bt,d::q,false},
	};

	std::vector<std::vector<beat_t>> ans_nbeat {
		{3_bt,2_bt, 3_bt+0.03125_bt} // There are 32 1/128'th notes in a 1/4 note; 1/32 == 0.03125
	};
	std::vector<std::vector<bar_t>> ans_nbar {
		{1_br,bar_t{2.0/3.0}, 1_br + bar_t{1.0/96.0}}  // 3 1/4 notes per bar => 3x32==96 1/128 notes
	};

	for (int i=0; i<ts.size(); ++i) {
		auto curr_ts = ts[i];
		for (int j=0; j<rp.size(); ++j) {
			auto curr_rp = rp[j];

			beat_t cum_nbeats {0};
			bar_t cum_nbars {0};
			for (int k=0; k<curr_rp.size(); ++k) {
				cum_nbeats += nbeat(curr_ts,curr_rp[k]);
				cum_nbars += nbar(curr_ts,curr_rp[k]);
			}
			EXPECT_TRUE(cum_nbeats == ans_nbeat[i][j]);
			EXPECT_TRUE(cum_nbars == ans_nbar[i][j]);

			EXPECT_TRUE(nbeat(curr_ts,cum_nbars) == cum_nbeats);
			EXPECT_TRUE(nbar(curr_ts,cum_nbeats) == cum_nbars);
		}
	}
}

