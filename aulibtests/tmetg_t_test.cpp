#include "pch.h"
#include "..\aulib\types\metg_t.h"
#include <vector>
#include <string>
#include <cmath>
#include <limits>

// Googletest does not support c++17, so i can't include my 
// header defining these functions.  
bool aprx_eq_gtest(double a, double b, int ulp=2) {
	auto e = std::numeric_limits<double>::epsilon();
	auto m = std::numeric_limits<double>::min();
	auto d = std::abs(a-b);
	auto s = std::abs(a+b);
	return (d <= e*s*ulp || d < m);
};

bool aprx_int_gtest(double a, int ulp=2) {
	double ra {std::round(a)};
	return aprx_eq_gtest(a, ra, ulp);
};

// No phase shift
TEST(metg_t_tests, ThreeFourZeroPhaseHQEdE) {
	std::vector<d_t> dt {d::h,d::q,d::ed,d::e};
	std::vector<beat_t> ph(dt.size(),beat_t{0});
	ts_t ts {beat_t{3},d::q};
	auto mg = tmetg_t(ts,dt,ph);
	// => m_btres = 0.25, m_period = 6 beats (2 bars)

	for (int i=0; i<48; ++i) {  // 48 => 12 beats => 2 bars
		for (const auto& e : dt) {
			bool tf = mg.allowed_at(e,beat_t{i*0.25});
			// A note of duration e should be allowed at any beat i where 
			// nbeat(ts,e)/i is an integer.  
			if (aprx_int_gtest(beat_t{i*0.25}/nbeat(ts,e))) {
				EXPECT_TRUE(tf);
			} else {
				EXPECT_FALSE(tf);
			}
		}
	}
}

// The (1/8). note is shifted _backwards_ by (1/32)'nd, which is half
// the duration of its dot, and forces the grid resolution to increase.  
TEST(metg_t_tests, ThreeFourNegSixteenthPhaseHQEdE) {
	std::vector<d_t> dt {d::h,d::q,d::ed,d::e};
	std::vector<beat_t> ph {0_bt,0_bt,-1*0.125_bt,0_bt};
	ts_t ts {beat_t{3},d::q};
	auto mg = tmetg_t(ts,dt,ph);
	// => m_btres = 0.125

	for (int i=0; i<48; ++i) {  // 48 => 12 beats => 2 bars
		for (int j=0; j<dt.size(); ++j) {
			bool tf = mg.allowed_at(dt[j],beat_t{i*0.25});
			// A note of duration e should be allowed at any beat i where 
			// nbeat(ts,e)/i is an integer.  
			if (aprx_int_gtest((beat_t{i*0.25}-ph[j])/nbeat(ts,dt[j]))) {
				EXPECT_TRUE(tf);
			} else {
				EXPECT_FALSE(tf);
			}
		}
	}
}

// All the notes have some sort of >= 0 shift
TEST(metg_t_tests, ThreeFourMultiPhaseShiftHQEdE) {
	std::vector<d_t> dt {d::h,d::q,d::ed,d::e};
	std::vector<beat_t> ph {0_bt,0.25_bt,0.5_bt,0.75_bt};
	ts_t ts {beat_t{3},d::q};
	auto mg = tmetg_t(ts,dt,ph);
	// => m_btres = 0.25

	for (int i=0; i<48; ++i) {  // 48 => 12 beats => 2 bars
		for (int j=0; j<dt.size(); ++j) {
			bool tf = mg.allowed_at(dt[j],beat_t{i*0.25});
			// A note of duration e should be allowed at any beat i where 
			// nbeat(ts,e)/i is an integer.  
			if (aprx_int_gtest((beat_t{i*0.25}-ph[j])/nbeat(ts,dt[j]))) {
				EXPECT_TRUE(tf);
			} else {
				EXPECT_FALSE(tf);
			}
		}
	}
}




// Creates an mg w/ all component nv's having ph = 0, then draw()'s an
// rp 10x.  Each rp is concatenated and a new mg is created from this huge
// rp.  draw() is called on the new mg, which should return an rp exactly
// the same as the rp which created it.  
TEST(metg_t_tests, ZeroPhaseBuildFromExistingRP) {
	std::vector<d_t> dt {d::h,d::q,d::ed,d::e};
	std::vector<beat_t> ph {0_bt,0_bt,0_bt,0_bt};
	ts_t ts {beat_t{3},d::q};
	auto mg = tmetg_t(ts,dt,ph);
	// => m_btres = 0.25
	
	std::vector<d_t> rp {};
	for (int i=0; i<10; ++i) {
		auto curr_rp = mg.draw();
		for (auto e : curr_rp) {
			rp.push_back(e);
		}
	}

	tmetg_t mg2 {ts,rp_t{ts,rp}};
	auto rp2 = mg2.draw();
	int i = 5;
	EXPECT_TRUE(rp.size() == rp2.size());

	for (int i=0; i<rp.size(); ++i) {
		EXPECT_TRUE(rp[i]==rp2[i]);
	}
}



