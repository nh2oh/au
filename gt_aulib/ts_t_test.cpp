//#include "pch.h"
#include "gtest/gtest.h"
#include "..\aulib\types\ts_t.h"
#include "..\aulib\types\beat_bar_t.h"
#include <vector>
#include <numeric>
#include <limits>


// Kahan summation
// Copied from au_algs_math.h b/c googletest does not support a modern 
// enough c++ to use <optional>
template<typename T>
struct ksum {
	T value {};
	T c {};

	ksum& operator+=(const T& rhs) {
		T y = rhs - c;
		T t = value + y;
		c = (t - value) - y;
		value = t;

		return *this;
	};
};



// Default ts is 4/4 simple
TEST(ts_t_tests, DefaultConstructor) {
	ts_t def_ts {};
	ts_t fourfour {4_bt,d::q};
	EXPECT_TRUE(def_ts == fourfour);
}


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


TEST(ts_t_tests, ThreeFourIterativeNbarCalculations) {
	ts_t ts {3_bt,d::q,false};

	//bar_t cum_nbars {0};
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

		/*
		cum_nbars += nbar(ts, d_t{d::q});

		if (cum_nbars.isexact()) {
			// TODO
			// This is done to prevent error accumulation in the 
			// repeated floating-point sum... it needs to eventually be
			// fixed.  
			cum_nbars = cum_nbars.full();
		}
		if (i%3==0) {
			bool isex = cum_nbars.isexact(); EXPECT_TRUE(isex);
			double frem = cum_nbars.fremain(); EXPECT_EQ(frem,1.0);
			bar_t nfull = cum_nbars.full(); EXPECT_EQ(nfull,(i/3)*1_br);
			bar_t nnext = cum_nbars.next(); EXPECT_EQ(nnext,(i/3)*1_br+1_br);
			EXPECT_EQ(cum_nbars,(i/3)*(1_br));
		}*/
	}

}



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

