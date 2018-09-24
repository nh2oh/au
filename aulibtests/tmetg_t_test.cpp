#include "pch.h"
#include "..\aulib\types\metg_t.h"
#include "..\aulib\util\au_random.h"
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <algorithm> // random_shuffle 

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
	bool tf_validates = mg.validate(); EXPECT_TRUE(tf_validates);
	// => m_btres = 0.25, m_period = 6 beats (2 bars)

	for (int i=0; i<48; ++i) {  // 48 => 12 beats => 2 bars
		for (const auto& e : dt) {
			bool tf = mg.onset_allowed_at(e,beat_t{i*0.25});
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
	EXPECT_TRUE(mg.validate());
	// => m_btres = 0.125

	for (int i=0; i<48; ++i) {  // 48 => 12 beats => 2 bars
		for (int j=0; j<dt.size(); ++j) {
			bool tf = mg.onset_allowed_at(dt[j],beat_t{i*0.25});
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
	bool tf = mg.validate(); EXPECT_TRUE(tf);
	// => m_btres = 0.25

	for (int i=0; i<48; ++i) {  // 48 => 12 beats => 2 bars
		for (int j=0; j<dt.size(); ++j) {
			tf = mg.onset_allowed_at(dt[j],beat_t{i*0.25});
			// A note of duration e should be allowed at any beat i where 
			// nbeat(ts,e)/i is an integer.  
			if (aprx_int_gtest((beat_t{i*0.25}-ph[j])/nbeat(ts,dt[j]))) {
				EXPECT_TRUE(tf);
				if (!tf) {
					auto z  =1+1;
				}
			} else {
				EXPECT_FALSE(tf);
				if (tf) {
					auto z  =1+1;
				}
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
	bool tf = mg.validate(); EXPECT_TRUE(tf);
	// => m_btres = 0.25, m_period = 6bts = 2 bars
	
	// Generate a big random rp by repeatedly draw()ing from the mg
	std::vector<d_t> rp {};
	for (int i=0; i<10; ++i) {  // 10 reps => 20 bars
		auto curr_rp = mg.draw();
		for (auto e : curr_rp) {
			rp.push_back(e);
		}
	}

	// An mg created from an rp has a pg where element corresponding to the
	// rp == 1.  Drawing an rp from the mg shoudl yield the same rp as 
	// created it.  
	tmetg_t mg2 {ts,rp_t{ts,rp}};
	EXPECT_TRUE(mg2.validate());
	auto rp2 = mg2.draw();

	EXPECT_TRUE(rp.size() == rp2.size());
	for (int i=0; i<rp.size(); ++i) {
		EXPECT_TRUE(rp[i]==rp2[i]);
	}
}


// Same as the test above, but the note durations are much smaller:
// 1/16 - 1/128.  The random rp is also much longer.  
// Meant as a test of the floating-point ops that count bars, beats,
// etc.  
TEST(metg_t_tests, ZeroPhaseBuildFromExistingRPSmallDurations) {
	std::vector<d_t> dt {d::sx,d::t,d::sf,d::ote};
	std::vector<beat_t> ph {0_bt,0_bt,0_bt,0_bt};
	ts_t ts {beat_t{3},d::q};
	auto mg = tmetg_t(ts,dt,ph);
	EXPECT_TRUE(mg.validate());
	// => m_btres = 0.25, m_period = 6bts = 2 bars
	
	std::vector<d_t> rp {};
	for (int i=0; i<100; ++i) {  // 100 reps => 200 bars
		auto curr_rp = mg.draw();
		for (auto e : curr_rp) {
			rp.push_back(e);
		}
	}

	tmetg_t mg2 {ts,rp_t{ts,rp}};
	EXPECT_TRUE(mg2.validate());
	auto rp2 = mg2.draw();
	EXPECT_TRUE(rp.size() == rp2.size());

	for (int i=0; i<rp.size(); ++i) {
		EXPECT_TRUE(rp[i]==rp2[i]);
	}
}


// Testing the ability of the rp ctor to calculate note-phases
TEST(metg_t_tests, FromRpWithNonzeroPhases1) {
	ts_t ts {4_bt,d::q};
	std::vector<d_t> vdt {
		d::sx,d::q,d::q,d::q,d::sx,d::e,  // ph d::q == d::sx
		d::sx,d::sx,d::q,d::q,d::q,d::e,  // ph d::q == d::e
		d::sx,d::sx,d::sx,d::q,d::q,d::q,d::sx,  // ph d::q == d::ed
		d::sx,d::sx,d::sx,d::sx,d::q,d::q,d::q};  // ph d::q == 0
	std::vector<teejee::nv_ph> allowed_levels {
		{d::q,d_t{0}},{d::q,d::sx},{d::q,d::e},{d::q,d::ed},
		{d::e,d_t{0}},
		{d::sx,d_t{0}}};

	// For an mg created w/ an rp constructed from vdt above, m_nvsph
	// (retruned by levels()) should be exactly allowed_levels (same elements
	// in the same order).  
	tmetg_t mg {ts,rp_t{ts,vdt}};
	bool tf = mg.validate(); EXPECT_TRUE(tf);
	tf = (mg.nbars() == 4_br); EXPECT_TRUE(tf);
	auto mg_lvls = mg.levels();
	EXPECT_TRUE(mg_lvls.size() == allowed_levels.size());
	for (int i=0; i<mg_lvls.size(); ++i) {
		tf = (mg_lvls[i].nv == allowed_levels[i].nv); EXPECT_TRUE(tf);
		tf = (mg_lvls[i].ph == allowed_levels[i].ph); EXPECT_TRUE(tf);
	}
	
	// Randomized versions of vdt may not contain all the phases of the q note
	// however, levels() should still be correctly sorted, and can't contain
	// _more_ q note phases than in allowed_levels.  
	// It might contain an e-note level w/ a sx-note phase, hence, not comparing
	// element-wise w/ levels_allowed.  
	for (int i=0; i<10; ++i) {
		std::random_shuffle(vdt.begin(),vdt.end());
		tmetg_t mgrand {ts,rp_t{ts,vdt}};
		tf = mgrand.validate();	EXPECT_TRUE(tf);
		tf =(mgrand.nbars() == 4_br); EXPECT_TRUE(tf);

		auto mgrand_lvls = mgrand.levels();
		//  Check the sorting
		for (int j=1; j<mgrand_lvls.size(); ++j) {  // Note:  Starts @ 1
			tf = (mgrand_lvls[j].nv <= mgrand_lvls[j-1].nv); EXPECT_TRUE(tf);
			if (mgrand_lvls[j].nv == mgrand_lvls[j-1].nv) {
				tf = (mgrand_lvls[j].ph > mgrand_lvls[j-1].ph);	EXPECT_TRUE(tf);
			}
		}
	}
}


// Testing the ability of the nv,ph ctor to calculate note-phases
TEST(metg_t_tests, FromNvPhWithNonzeroPhases) {
	auto ts = ts_t{4_bt,d::q};
	std::vector<d_t> vdt     {d::h,d::q,d::q,        d::q,d::q,  d::q,d::e};
	std::vector<beat_t> ph1  {0_bt,0_bt,beat_t{-1.5},1_bt,0.5_bt,2_bt,0_bt};

	// The smallest nv is d::e and the phases all divide evenly into some 
	// number of 1/2 -beats => d::e, so the only phase values for the q-note
	// are 0 and d::e.  
	std::vector<teejee::nv_ph> allowed_levels {
		{d::h,d_t{0}},
		{d::q,d_t{0}},{d::q,d::e},
		{d::e,d_t{0}}};

	// Phase-set 1
	for (int i=0; i<10; ++i) {
		auto mg = tmetg_t(ts,vdt,ph1);
		bool tf = mg.validate(); EXPECT_TRUE(tf);
		auto mg_lvls = mg.levels();
		EXPECT_TRUE(mg_lvls.size() == allowed_levels.size());
		for (int j=0; j<mg_lvls.size(); ++j) {
			tf = (mg_lvls[j].nv == allowed_levels[j].nv); EXPECT_TRUE(tf);
			tf = (mg_lvls[j].ph == allowed_levels[j].ph); 
			EXPECT_TRUE(tf);
		}
		std::random_shuffle(ph1.begin()+1,ph1.end()-1);
		// Note the +1, -1; need to exclude the h and e nts
	}

	// Phase set 2
	//               vdt     {d::h, d::q,         d::q,         d::q,   d::q,  d::q,   d::e};
	std::vector<beat_t> ph2  {0_bt, beat_t{-800}, beat_t{-1.5}, 100_bt, 0_bt,  0.5_bt, 0_bt};
	for (int i=0; i<10; ++i) {
		auto mg = tmetg_t(ts,vdt,ph2);
		bool tf = mg.validate(); EXPECT_TRUE(tf);
		auto mg_lvls = mg.levels();
		EXPECT_TRUE(mg_lvls.size() == allowed_levels.size());
		for (int j=0; j<mg_lvls.size(); ++j) {
			tf = (mg_lvls[j].nv == allowed_levels[j].nv); EXPECT_TRUE(tf);
			tf = (mg_lvls[j].ph == allowed_levels[j].ph); EXPECT_TRUE(tf);
		}
		std::random_shuffle(ph2.begin()+1,ph2.end()-1);
		// Note the +1, -1; need to exclude the h and e nts
	}
}


TEST(metg_t_tests, SpanPossibleThreeFourSet1) {
	ts_t ts {3_bt,d::q};
	std::vector<d_t> vdt {d::w,d::h,d::q};
	std::vector<beat_t> ph(vdt.size(),0_bt);
	tmetg_t mg {ts,vdt,ph};
	bool tf_validates = mg.validate(); EXPECT_TRUE(tf_validates);

	// The smallest member of m_nvsph is the q-nt => 1 beat, thus all
	// even numbers of beats should be possible, and any fractional number
	// of beats should not.  
	std::vector<beat_t> expect_possible {
		0_bt,1_bt,2_bt,3_bt,100_bt,1000_bt,731_bt};
	for (auto e : expect_possible) {
		EXPECT_TRUE(mg.span_possible(e));
		EXPECT_TRUE(mg.span_possible(nbar(ts,e)));
	}

	std::vector<beat_t> expect_impossible {
		0.125_bt,0.375_bt,0.625_bt,0.875_bt,100.125_bt,1000.375_bt,731.875_bt,
		100.625_bt,731.625_bt,1000.125_bt,
		0.25_bt, 0.5_bt, 0.75_bt,100.25_bt,731.75_bt,1000.5_bt};
	for (auto e : expect_impossible) {
		EXPECT_FALSE(mg.span_possible(e));
		EXPECT_FALSE(mg.span_possible(nbar(ts,e)));
	}
}


TEST(metg_t_tests, SpanPossibleThreeFourSet2) {
	ts_t ts {3_bt,d::q};
	std::vector<d_t> vdt {d::w,d::h};
	std::vector<beat_t> ph(vdt.size(),0_bt);
	tmetg_t mg {ts,vdt,ph};
	bool tf_validates = mg.validate(); EXPECT_TRUE(tf_validates);

	// The smallest member of m_nvsph is the h-nt => 2 beats => 2/3 bars.
	// Thus, all beats numbers that are multiples of 2 should be possible
	// and any odd number of beats should not.  
	std::vector<beat_t> expect_possible {
		0_bt,2_bt,4_bt,6_bt,100_bt,1000_bt,732_bt};
	for (auto e : expect_possible) {
		EXPECT_TRUE(mg.span_possible(e));
		EXPECT_TRUE(mg.span_possible(nbar(ts,e)));
	}

	std::vector<beat_t> expect_impossible {
		0.125_bt,0.375_bt,0.625_bt,0.875_bt,100.125_bt,1000.375_bt,731.875_bt,
		100.625_bt,731.625_bt,1000.125_bt,
		0.25_bt, 0.5_bt, 0.75_bt,100.25_bt,731.75_bt,1000.5_bt,
		1_bt,3_bt,5_bt,9_bt,11_bt,101_bt};
	for (auto e : expect_impossible) {
		EXPECT_FALSE(mg.span_possible(e));
		EXPECT_FALSE(mg.span_possible(nbar(ts,e)));
	}
}

TEST(metg_t_tests, SetPgSet1) {
	ts_t ts {3_bt,d::q};
	std::vector<d_t> vdt {d::h,d::q,d::e};
	std::vector<beat_t> vph {0_bt,0_bt,0_bt};
	teejee tg {ts,vdt,vph};
	tmetg_t mg {ts,vdt,vph};

	int Ntest = 100;
	auto newp = urandd(Ntest,0,2);
	auto btidx = urandi(Ntest,0,nbeat(tg.ts(),tg.period())/tg.gres()-1);
	auto rowidx = urandi(Ntest,0,mg.levels().size()-1);
	for (int i=0; i<btidx.size(); ++i) {
		auto curr_bt = tg.gres()*btidx[i];
		auto curr_lvl = mg.levels()[rowidx[i]];
		if (newp[i] < 1) {newp[i] = 0.0;}
		auto tf = mg.set_pg(curr_lvl,curr_bt,newp[i]);
		EXPECT_TRUE(mg.validate());
		if (!tf) { continue; }

		if (newp[i] == 0.0) {
			EXPECT_FALSE(mg.onset_allowed_at(curr_lvl.nv, curr_bt));
		}
	}
}


TEST(metg_t_tests, SetPgByRowAllZeroSet1) {
	ts_t ts {3_bt,d::q};
	std::vector<d_t> vdt {d::w,d::hd,d::h,d::qd,d::q,d::ed,d::e};
	std::vector<beat_t> vph {0_bt,0_bt,0_bt,0_bt,0_bt,0_bt,0_bt};
	teejee tg {ts,vdt,vph};
	tmetg_t mg {ts,vdt,vph};

	int ncols = nbeat(tg.ts(),tg.period())/tg.gres();
	for (int i=0; i<mg.levels().size(); ++i) {
		auto zero_row = std::vector<double>(ncols,0.0);
		auto curr_lvl = mg.levels()[i];

		EXPECT_TRUE(mg.set_pg(curr_lvl,zero_row));
		EXPECT_TRUE(mg.validate());
	}
}


// 4/4, w,h,q => Period = 1 bar
// Tests of functionality that does not involve modifying the pg
TEST(metg_t_tests, FourFourW0H0Q0Z0) {
	ts_t ts {4_bt,d::q};
	std::vector<d_t> vdt {d::w,d::h,d::q};
	std::vector<beat_t> vph {0_bt,0_bt,0_bt};
	tmetg_t mg {ts,vdt,vph};
	EXPECT_TRUE(mg.validate());
	EXPECT_TRUE(mg.nbars()==1_br);
	EXPECT_TRUE(mg.ts()==ts);

	// Slice() and factor()
	EXPECT_TRUE(mg.factor().size() == 1);  // Does not factor
		EXPECT_TRUE(mg.factor()[0] == mg);
	auto mg2x = mg.slice(0_bt,8_bt);  // Exactly 2 periods
	EXPECT_TRUE(mg2x.validate());
	EXPECT_TRUE(mg2x.nbars()==2_br);
	EXPECT_TRUE(mg2x.ts()==ts);
	EXPECT_TRUE(mg2x.factor().size() == 2);  // Expect will factor into 2 mg's
		EXPECT_TRUE(mg2x.factor()[0] == mg);
		EXPECT_FALSE(mg2x.factor()[1] == mg);  // [i].m_btstart==4, mg.m_btstart==0

	// Onsets are allowed @ every beat, but not between beats (=> tg.gres()==1bt)
	for (int i=-50; i<50; ++i) {
		beat_t curr_bt {0.5*i};
		bar_t curr_bar = nbar(ts,curr_bt);
		// .onset_allowed_at(beat-number) and .onset_allowed_at(d_t,beat-number)
		if (aprx_int_gtest(curr_bt/1_bt)) {
			EXPECT_TRUE(mg.onset_allowed_at(curr_bt));
			EXPECT_TRUE(mg.onset_allowed_at(d_t{d::q},curr_bt));
		} else {
			EXPECT_FALSE(mg.onset_allowed_at(curr_bt));
			EXPECT_FALSE(mg.onset_allowed_at(d_t{d::q},curr_bt));
		}
		if (aprx_int_gtest(curr_bar/1_br)) {
			EXPECT_TRUE(mg.onset_allowed_at(d_t{d::w},curr_bt));
		} else {
			EXPECT_FALSE(mg.onset_allowed_at(d_t{d::w},curr_bt));
		}
		if (aprx_int_gtest(curr_bt/2_bt)) {
			EXPECT_TRUE(mg.onset_allowed_at(d_t{d::h},curr_bt));
		} else {
			EXPECT_FALSE(mg.onset_allowed_at(d_t{d::h},curr_bt));
		}

		// .span_possible(number-of-beats) and .span_possible(number-of-bars)
		if (curr_bt < 0_bt) {
			EXPECT_FALSE(mg.span_possible(curr_bt));
			EXPECT_FALSE(mg.span_possible(curr_bar));
		} else if (curr_bt >= 0_bt && aprx_int_gtest(curr_bt/1_bt)) {
			EXPECT_TRUE(mg.span_possible(curr_bt));
			EXPECT_TRUE(mg.span_possible(curr_bar));
		}
	}

	// enumerate()
	// 5 rp's total:  w; h,h; h,q,q; q,q,h; q,q,q,q
	auto allrps_rpp = mg.enumerate();
	std::vector<std::vector<d_t>> allrps_rp {};
	for (int i=0; i<allrps_rpp.size(); ++i) {
		allrps_rp.push_back(allrps_rpp[i].rp);
	}
	std::vector<std::vector<d_t>> allrps_ans {{d::w},{d::h,d::h},{d::q,d::q,d::h},
		{d::h,d::q,d::q},{d::q,d::q,d::q,d::q}};
	EXPECT_TRUE(allrps_rp.size() == allrps_ans.size());
	for (int i=0; i<allrps_ans.size(); ++i) {
		EXPECT_TRUE(std::find(allrps_rp.begin(),allrps_rp.end(),allrps_ans[i]) != allrps_rp.end());
	}

	// draw() should always pull from the allrps_ans set
	for (int i=0; i<50; ++i) {
		auto curr_rrp = mg.draw();
		EXPECT_TRUE(std::find(allrps_ans.begin(),allrps_ans.end(),curr_rrp) != allrps_ans.end());
	}
}



// 4/4; w,h,q; 1 bt phase shift applied to w => Period = 1 bar, but the
// rp containing the w note == 1.25 bars and is longer than all the others.  
// Tests of functionality that does not involve modifying the pg
TEST(metg_t_tests, FourFourW1H0Q0) {
	ts_t ts {4_bt,d::q};
	std::vector<d_t> vdt {d::w,d::h,d::q};
	std::vector<beat_t> vph {1_bt,0_bt,0_bt};
	tmetg_t mg {ts,vdt,vph};
	EXPECT_TRUE(mg.validate());
	// EXPECT_TRUE(mg.nbars()==1_br);  // Not true for all enumerations
	EXPECT_TRUE(mg.ts()==ts);

	// Slice() and factor()
	EXPECT_TRUE(mg.factor().size() == 1);  // Does not factor
		EXPECT_TRUE(mg.factor()[0] == mg);
	auto mg2x = mg.slice(0_bt,8_bt);  // Exactly 2 periods
	EXPECT_TRUE(mg2x.validate());
	// EXPECT_TRUE(mg2x.nbars()==2_br);  // Not true for all enumerations
	EXPECT_TRUE(mg2x.ts()==ts);
	EXPECT_TRUE(mg2x.factor().size() == 1);  // Should not factor
		EXPECT_TRUE(mg2x.factor()[0] == mg2x);

	// Onsets are allowed @ every beat, but not between beats (=> tg.gres()==1bt)
	for (int i=-50; i<50; ++i) {
		beat_t curr_bt {0.5*i};
		bar_t curr_bar = nbar(ts,curr_bt);
		// .onset_allowed_at(beat-number) and .onset_allowed_at(d_t,beat-number)
		if (aprx_int_gtest(curr_bt/1_bt)) {
			EXPECT_TRUE(mg.onset_allowed_at(curr_bt));
			EXPECT_TRUE(mg.onset_allowed_at(d_t{d::q},curr_bt));
		} else {
			EXPECT_FALSE(mg.onset_allowed_at(curr_bt));
			EXPECT_FALSE(mg.onset_allowed_at(d_t{d::q},curr_bt));
		}
		if (aprx_int_gtest((curr_bar-0.25_br)/1_br)) {
			EXPECT_TRUE(mg.onset_allowed_at(d_t{d::w},curr_bt));
		} else {
			EXPECT_FALSE(mg.onset_allowed_at(d_t{d::w},curr_bt));
		}
		if (aprx_int_gtest(curr_bt/2_bt)) {
			EXPECT_TRUE(mg.onset_allowed_at(d_t{d::h},curr_bt));
		} else {
			EXPECT_FALSE(mg.onset_allowed_at(d_t{d::h},curr_bt));
		}

		// .span_possible(number-of-beats) and .span_possible(number-of-bars)
		if (curr_bt < 0_bt) {
			EXPECT_FALSE(mg.span_possible(curr_bt));
			EXPECT_FALSE(mg.span_possible(curr_bar));
		} else if (curr_bt >= 0_bt && aprx_int_gtest(curr_bt/1_bt)) {
			EXPECT_TRUE(mg.span_possible(curr_bt));
			EXPECT_TRUE(mg.span_possible(curr_bar));
		}
	}

	// enumerate()
	// 5 rp's total:  h,h; h,q,q; q,q,h; q,w; q,q,q,q
	auto allrps_rpp = mg.enumerate();
	std::vector<std::vector<d_t>> allrps_rp {};
	for (int i=0; i<allrps_rpp.size(); ++i) {
		allrps_rp.push_back(allrps_rpp[i].rp);
	}
	std::vector<std::vector<d_t>> allrps_ans {{d::q,d::w},{d::h,d::h},{d::q,d::q,d::h},
		{d::h,d::q,d::q},{d::q,d::q,d::q,d::q}};
	EXPECT_TRUE(allrps_rp.size() == allrps_ans.size());
	for (int i=0; i<allrps_ans.size(); ++i) {
		EXPECT_TRUE(std::find(allrps_rp.begin(),allrps_rp.end(),allrps_ans[i]) != allrps_rp.end());
	}

	// draw() should always pull from the allrps_ans set
	for (int i=0; i<50; ++i) {
		auto curr_rrp = mg.draw();
		EXPECT_TRUE(std::find(allrps_ans.begin(),allrps_ans.end(),curr_rrp) != allrps_ans.end());
	}
}


// 4/4; w,h,q; 0.5 bt phase shift applied to w => Period = 1 bar, but the
// rp containing the w note == 1.125 bars and is longer than all the others.  
// This w note is an orphan and will never appear in an rp.  
// Tests of functionality that does not involve modifying the pg
TEST(metg_t_tests, FourFourW05H0Q0) {
	ts_t ts {4_bt,d::q};
	std::vector<d_t> vdt {d::w,d::h,d::q};
	std::vector<beat_t> vph {0.5_bt,0_bt,0_bt};
	tmetg_t mg {ts,vdt,vph};
	EXPECT_TRUE(mg.validate());
	// EXPECT_TRUE(mg.nbars()==1_br);  // Not true for all enumerations
	EXPECT_TRUE(mg.ts()==ts);

	// Slice() and factor()
	EXPECT_TRUE(mg.factor().size() == 1);  // Does not factor
		EXPECT_TRUE(mg.factor()[0] == mg);
	auto mg2x = mg.slice(0_bt,8_bt);  // Exactly 2 periods
	EXPECT_TRUE(mg2x.validate());
	// EXPECT_TRUE(mg2x.nbars()==2_br);  // Not true for all enumerations
	EXPECT_TRUE(mg2x.ts()==ts);
	EXPECT_TRUE(mg2x.factor().size() == 1);  // Should not factor
		EXPECT_TRUE(mg2x.factor()[0] == mg2x);

	// Onsets are allowed @ every beat and at every beat that is a multiple of
	// nbeat(ts,d::w)+0.5_bt, which is where the w note occurs.  
	for (int i=-50; i<50; ++i) {
		beat_t curr_bt {0.25*i};  // Note:  Smaller than usual step-resolution of 0.25
		bar_t curr_bar = nbar(ts,curr_bt);
		// .onset_allowed_at(beat-number) and .onset_allowed_at(d_t,beat-number)
		//curr_bt=4.5_bt;
		bool tf = mg.onset_allowed_at(curr_bt);
		if (aprx_int_gtest(curr_bt/1_bt) || aprx_int_gtest((curr_bt-0.5_bt)/nbeat(ts,d_t{d::w}))) {
			EXPECT_TRUE(mg.onset_allowed_at(curr_bt));
		} else {
			EXPECT_FALSE(mg.onset_allowed_at(curr_bt));
		}

		tf = mg.onset_allowed_at(d_t{d::w},curr_bt);
		if (aprx_int_gtest((curr_bt-0.5_bt)/nbeat(ts,d_t{d::w}))) {
			EXPECT_TRUE(mg.onset_allowed_at(d_t{d::w},curr_bt));
		} else {
			EXPECT_FALSE(mg.onset_allowed_at(d_t{d::w},curr_bt));
		}
		if (aprx_int_gtest(curr_bt/2_bt)) {
			EXPECT_TRUE(mg.onset_allowed_at(d_t{d::h},curr_bt));
		} else {
			EXPECT_FALSE(mg.onset_allowed_at(d_t{d::h},curr_bt));
		}
		if (aprx_int_gtest(curr_bt/1_bt)) {
			EXPECT_TRUE(mg.onset_allowed_at(d_t{d::q},curr_bt));
		} else {
			EXPECT_FALSE(mg.onset_allowed_at(d_t{d::q},curr_bt));
		}

		// .span_possible(number-of-beats) and .span_possible(number-of-bars)
		if (curr_bt < 0_bt) {
			EXPECT_FALSE(mg.span_possible(curr_bt));
			EXPECT_FALSE(mg.span_possible(curr_bar));
		} else if (curr_bt >= 0_bt && aprx_int_gtest(curr_bt/1_bt)) {
			EXPECT_TRUE(mg.span_possible(curr_bt));
			EXPECT_TRUE(mg.span_possible(curr_bar));
		}
	}

	// enumerate()
	// 4 rp's total:  h,h; h,q,q; q,q,h; q,q,q,q
	auto allrps_rpp = mg.enumerate();
	std::vector<std::vector<d_t>> allrps_rp {};
	for (int i=0; i<allrps_rpp.size(); ++i) {
		allrps_rp.push_back(allrps_rpp[i].rp);
	}
	std::vector<std::vector<d_t>> allrps_ans {{d::h,d::h},{d::q,d::q,d::h},
		{d::h,d::q,d::q},{d::q,d::q,d::q,d::q}};
	EXPECT_TRUE(allrps_rp.size() == allrps_ans.size());
	for (int i=0; i<allrps_ans.size(); ++i) {
		EXPECT_TRUE(std::find(allrps_rp.begin(),allrps_rp.end(),allrps_ans[i]) != allrps_rp.end());
	}

	// draw() should always pull from the allrps_ans set
	for (int i=0; i<50; ++i) {
		auto curr_rrp = mg.draw();
		EXPECT_TRUE(std::find(allrps_ans.begin(),allrps_ans.end(),curr_rrp) != allrps_ans.end());
	}
}

