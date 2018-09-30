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

std::vector<std::vector<double>> transpose_gtest(const std::vector<std::vector<double>>& m) {
	if (m.size() == 0) { return std::vector<std::vector<double>> {}; }
	auto n1 = m.size();  // 1 => "dimension 1"
	auto n2 = m[0].size();  // 2 => "dimension 2"

	std::vector<std::vector<double>> res(n2,std::vector<double>(n1, 0.0));
	for (auto i=0; i<n1; ++i) {
		if (m[i].size() != n2) { return std::vector<std::vector<double>> {}; }
		for (auto j=0; j<n2; ++j) {
			res[j][i] = m[i][j];
		}
	}

	return res;
}


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
			} else {
				EXPECT_FALSE(tf);
			}
		}
	}
}


// Creates an mg w/ all component nv's having ph = 0, then draw()'s an
// rp 10x.  Each rp is concatenated and a new mg is created from this "huge"
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
	std::vector<d_t> vdt {};
	for (int i=0; i<10; ++i) {  // 10 reps => 20 bars
		auto curr_rp = mg.draw();
		for (auto e : curr_rp) {
			vdt.push_back(e);
		}
	}

	// An mg created from an rp has a pg where each element corresponding to the
	// rp == 1.  Drawing an rp from the mg should therefore yield the same rp 
	// as that which created it.  
	// The new mg should also have the same levels() as the parent mg from which
	// the rp was drawn().  This is a consequence of the fact that default-constructed
	// mg's are extendable (that is, rp's can be concatenated).  Were this not true, 
	// the mg generated from the rp could contain phase-shifted levels() of the
	// parent mg.  
	tmetg_t mg2 {ts,rp_t{ts,vdt}};
	EXPECT_TRUE(mg2.validate());
	auto vdt2 = mg2.draw();

	EXPECT_TRUE(vdt.size() == vdt2.size());
	for (int i=0; i<vdt.size(); ++i) {
		EXPECT_TRUE(vdt[i]==vdt2[i]);
	}

	EXPECT_TRUE(mg2!=mg); // mg2.m_pg is much larger than mg.m_pg

	// Any level appearing in mg2 should also appear in mg1.  
	// *Probably* mg2.levels()==mg.levels(), but occasionally 10 calls to draw()
	// will not include one or more levels.  
	auto mg1_lvls = mg.levels();
	auto mg2_lvls = mg2.levels();
	for (auto lvl2 : mg2_lvls) {
		bool found_in_mg1 = false;
		for (auto lvl1 : mg1_lvls) {
			if (lvl2 == lvl1) {
				found_in_mg1 = true;
				break;
			}
		}
		EXPECT_TRUE(found_in_mg1);
	}

}


// Same as the test above ("ZeroPhaseBuildFromExistingRP"), but the note durations
// are much smaller:
// 1/16 - 1/128.  The random rp is also much longer.  
// Meant as both a test of tmetg_t, but also the floating-point ops that count bars, beats,
// etc.  
TEST(metg_t_tests, ZeroPhaseBuildFromExistingRPSmallDurations) {
	std::vector<d_t> dt {d::sx,d::t,d::sf,d::ote};
	std::vector<beat_t> ph {0_bt,0_bt,0_bt,0_bt};
	ts_t ts {beat_t{3},d::q};
	auto mg = tmetg_t(ts,dt,ph);
	EXPECT_TRUE(mg.validate());
	// => m_btres = 0.25, m_period = 6bts = 2 bars
	
	std::vector<d_t> vdt {};
	for (int i=0; i<100; ++i) {  // 100 reps => 200 bars
		auto curr_rp = mg.draw();
		for (auto e : curr_rp) {
			vdt.push_back(e);
		}
	}

	tmetg_t mg2 {ts,rp_t{ts,vdt}};
	EXPECT_TRUE(mg2.validate());
	auto vdt2 = mg2.draw();
	EXPECT_TRUE(vdt.size() == vdt2.size());

	for (int i=0; i<vdt.size(); ++i) {
		EXPECT_TRUE(vdt[i]==vdt2[i]);
	}

	EXPECT_TRUE(mg2!=mg); // mg2.m_pg is much larger than mg.m_pg

	// Any level appearing in mg2 should also appear in mg1.  
	// *Probably* mg2.levels()==mg.levels(), but occasionally 10 calls to draw()
	// will not include one or more levels.  
	auto mg1_lvls = mg.levels();
	auto mg2_lvls = mg2.levels();
	for (auto lvl2 : mg2_lvls) {
		bool found_in_mg1 = false;
		for (auto lvl1 : mg1_lvls) {
			if (lvl2 == lvl1) {
				found_in_mg1 = true;
				break;
			}
		}
		EXPECT_TRUE(found_in_mg1);
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

// Tests span_possible() for various beat_t and bar_t numbers
// 3/4, w,h,q w/ 0-phase
TEST(metg_t_tests, WhichRpSpansPossibleThreeFourSet1) {
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

// Tests span_possible() for various beat_t and bar_t numbers
// 3/4, w,h w/ 0-phase
TEST(metg_t_tests, WhichRpSpansPossibleThreeFourSet2) {
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

// Tests set_pg(level,beat)
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


// Tests set_pg(level,row) by setting entire rows to zero
// It should be possible to zero out the pg by setting entire rows to 0.  It is
// not possible to do this element by element via set_pg(level,beat) because doing so 
// will create zero pointers and orphans; zero-pointers will cause validate()
// to fail.  
// This changes the effective period of the tmetg_t, making it different from 
// that of m_tg.  
TEST(metg_t_tests, SetPgByRowAllZeroSet1) {
	ts_t ts {3_bt,d::q};
	std::vector<d_t> vdt {d::w,d::hd,d::h,d::qd,d::q,d::ed,d::e};
	std::vector<beat_t> vph {0_bt,0_bt,0_bt,0_bt,0_bt,0_bt,0_bt};
	teejee tg {ts,vdt,vph};
	tmetg_t mg {ts,vdt,vph};
	EXPECT_TRUE(mg.validate());

	int ncols = nbeat(tg.ts(),tg.period())/tg.gres();
	for (int i=0; i<mg.levels().size(); ++i) {
		auto zero_row = std::vector<double>(ncols,0.0);
		auto curr_lvl = mg.levels()[i];

		EXPECT_TRUE(mg.set_pg(curr_lvl,zero_row));
		EXPECT_TRUE(mg.validate());
	}
}


// Tests the ctor that creates the tmetg_t from a user-specified pg.  
// The test then uses set_exact_length to extend the pg to 1.5 bars, and remove
// the second w-note (occuring @ beat 4), bhich points beyond 6 beats.  
TEST(metg_t_tests, ConstructFromPgFourFourWHQThenSliceTo6bts) {
	beat_t target_bts {6};
	bar_t target_brs {1.5};
	ts_t ts1 {4_bt,d::q};
	teejee tg1 {ts1,{d::w,d::h,d::q},{0_bt,0_bt,0_bt}};

	std::vector<std::vector<double>> pg1 {
		{1,0,0,0},
		{1,0,1,0},
		{1,1,1,1}
	};
	auto pg1t = transpose_gtest(pg1);

	tmetg_t mg1 {ts1,tg1.levels(),pg1t};
	auto tf1 = mg1.validate();  EXPECT_TRUE(tf1);
	EXPECT_TRUE(mg1.nbars() == 1_br);
	EXPECT_TRUE(mg1.ts() == ts1);

	auto mg2 = mg1.slice(0_bt,target_bts);
	mg2.set_length_exact(target_bts);
	auto tf2 = mg2.validate();  EXPECT_TRUE(tf2);
	auto nb2 = mg2.nbars(); EXPECT_TRUE(nb2 == target_brs);
	EXPECT_TRUE(mg2.ts() == ts1);

	auto mg3=mg1; mg3.set_length_exact(target_bts);
	auto tf3 = mg3.validate();  EXPECT_TRUE(tf3);
	auto nb3 = mg3.nbars(); EXPECT_TRUE(nb3 == target_brs);
	EXPECT_TRUE(mg3.ts() == ts1);

	// Extension w/ slice(), then calling set_length_exact() should yield the same
	// result as a single call to set_length_exact() (which calls slice() if the m_pg
	// is too short).  
	EXPECT_TRUE(mg2==mg3);

	auto allrps3 = mg3.enumerate();
	EXPECT_TRUE(allrps3.size() == 10);  // 5 possibilities for bar 1, 2 for partial bar 2 (h,q)
	for (int i=0; i<allrps3.size(); ++i) {
		auto curr_tot_nbts = nbeat(mg3.ts(),allrps3[i].rp);
		auto curr_tot_nbars = nbar(mg3.ts(),allrps3[i].rp);
		bool tf_nbt = curr_tot_nbts == target_bts; EXPECT_TRUE(tf_nbt);
		bool tf_nbr = curr_tot_nbars == target_brs; EXPECT_TRUE(tf_nbr);
	}
}


// Tests the ctor that creates the tmetg_t from a user-specified pg.  
// The test then uses set_exact_length to extend the pg to 1.5 bars, and remove
// the second w-note (occuring @ beat 4), and the 4'th h-note, both of which 
// point beyond 7 beats.  
TEST(metg_t_tests, ConstructFromPgFourFourWHQThenSliceTo7bts) {
	beat_t target_bts {7};
	bar_t target_brs {1.75};
	ts_t ts1 {4_bt,d::q};
	teejee tg1 {ts1,{d::w,d::h,d::q},{0_bt,0_bt,0_bt}};

	std::vector<std::vector<double>> pg1 {
		{1,0,0,0},
		{1,0,1,0},
		{1,1,1,1}
	};
	auto pg1t = transpose_gtest(pg1);

	tmetg_t mg1 {ts1,tg1.levels(),pg1t};
	auto tf1 = mg1.validate();  EXPECT_TRUE(tf1);
	EXPECT_TRUE(mg1.nbars() == 1_br);
	EXPECT_TRUE(mg1.ts() == ts1);

	auto mg2 = mg1.slice(0_bt,target_bts);
	mg2.set_length_exact(target_bts);
	auto tf2 = mg2.validate();  EXPECT_TRUE(tf2);
	auto nb2 = mg2.nbars(); EXPECT_TRUE(nb2 == target_brs);
	EXPECT_TRUE(mg2.ts() == ts1);

	auto mg3=mg1; mg3.set_length_exact(target_bts);
	auto tf3 = mg3.validate();  EXPECT_TRUE(tf3);
	auto nb3 = mg3.nbars(); EXPECT_TRUE(nb3 == target_brs);
	EXPECT_TRUE(mg3.ts() == ts1);

	// Extension w/ slice(), then calling set_length_exact() should yield the same
	// result as a single call to set_length_exact() (which calls slice() if the m_pg
	// is too short).  
	EXPECT_TRUE(mg2==mg3);

	auto allrps3 = mg3.enumerate();
	EXPECT_TRUE(allrps3.size() == 10);  // 5 possibilities for bar 1, 2 for partial bar 2 (qqq,hqq)
	for (int i=0; i<allrps3.size(); ++i) {
		auto curr_tot_nbts = nbeat(mg3.ts(),allrps3[i].rp);
		auto curr_tot_nbars = nbar(mg3.ts(),allrps3[i].rp);
		bool tf_nbt = curr_tot_nbts == target_bts; EXPECT_TRUE(tf_nbt);
		bool tf_nbr = curr_tot_nbars == target_brs; EXPECT_TRUE(tf_nbr);
	}
}

// Tests of functionality that does not involve modifying the pg
// Mostly tests of span_possible()/onset_allowed_at() and enumerate()/draw()
// for short mg's w/ varying levels and phases.  "Short" means small enough 
// that i can manually enter all possible rps.  
// 
TEST(metg_t_tests, AssortedTestsShortMgsVaryingTsNvsPhases) {
	
	struct test_set {
		ts_t ts {};
		std::vector<d_t> vdt {};
		std::vector<beat_t> vph {};

		beat_t slice_to_beat {};
		int beat_iter_start {};
		int beat_iter_end {};
		beat_t beat_iter_step {};
		int niter_draw {};

		int ans_factor_size {};
		int ans_slice_factor_size {};
		std::vector<std::vector<d_t>> ans_all_rps {};
	};

	std::vector<test_set> tsts {
		{
			ts_t {4_bt,d::q}, {d::w,d::h,d::q}, {0_bt,0_bt,0_bt},
			8_bt, -50, 50, 0.25_bt, 50,
			1, 2, {{d::w},{d::h,d::h},{d::q,d::q,d::h},{d::h,d::q,d::q},{d::q,d::q,d::q,d::q}}
		},
		{
			// 4/4; w,h,q; 0.5 bt phase shift applied to w => Period = 1 bar, but the
			// rp containing the w note == 1.125 bars and is longer than all the others.  
			// This w note is an orphan and will never appear in an rp.  
			ts_t {4_bt,d::q}, {d::w,d::h,d::q}, {0.5_bt,0_bt,0_bt},
			16_bt, -50, 50, 0.25_bt, 50,
			1, 1, {{d::h,d::h},{d::q,d::q,d::h},{d::h,d::q,d::q},{d::q,d::q,d::q,d::q}}
		},
		{
			ts_t {4_bt,d::q}, {d::w,d::h,d::q}, {1_bt,0_bt,0_bt},
			8_bt, -50, 50, 0.5_bt, 50, 
			1, 1, {{d::q,d::w},{d::h,d::h},{d::q,d::q,d::h},{d::h,d::q,d::q},{d::q,d::q,d::q,d::q}}
		},
		{
			// Shifting the w note _back_ by 3 bts should be the same as shifting it _forward_
			// by 1 bt.  
			ts_t {4_bt,d::q}, {d::w,d::h,d::q}, {-1*3_bt,0_bt,0_bt},
			8_bt, -50, 50, 0.5_bt, 50, 
			1, 1, {{d::q,d::w},{d::h,d::h},{d::q,d::q,d::h},{d::h,d::q,d::q},{d::q,d::q,d::q,d::q}}
		},
		{
			// Shifting the w note _back_ by 16 bts should be the same as not shifting it
			ts_t {4_bt,d::q}, {d::w,d::h,d::q}, {-1*16_bt,0_bt,0_bt},
			8_bt, -50, 50, 0.25_bt, 50,
			1, 2, {{d::w},{d::h,d::h},{d::q,d::q,d::h},{d::h,d::q,d::q},{d::q,d::q,d::q,d::q}}
		}
	};

	for (int t=0; t<tsts.size(); ++t) {
		auto ts = tsts[t].ts;
		auto vdt = tsts[t].vdt;
		auto vph = tsts[t].vph;
		tmetg_t mg {ts,vdt,vph};
		EXPECT_TRUE(mg.validate());

		EXPECT_TRUE(mg.ts()==ts);

		// Slice() and factor()
		EXPECT_TRUE(mg.factor().size() == tsts[t].ans_factor_size);  
		if (tsts[t].ans_factor_size == 1) { // Does not factor
			EXPECT_TRUE(mg.factor()[0] == mg);
		}
		auto mgsl = mg.slice(0_bt,tsts[t].slice_to_beat);
		EXPECT_TRUE(mgsl.validate());
		EXPECT_TRUE(mgsl.ts()==ts);
		EXPECT_TRUE(mgsl.factor().size() == tsts[t].ans_slice_factor_size);
		if (tsts[t].ans_slice_factor_size == 1) { // Does not factor
			EXPECT_TRUE(mgsl.factor()[0] == mgsl);
		}

		// Onsets are allowed @ every beat that is a multiple of (curr_bt-ph)/d_t
		for (int i=tsts[t].beat_iter_start; i<tsts[t].beat_iter_end; ++i) {
			auto curr_bt = tsts[t].beat_iter_step*i;
			bar_t curr_bar = nbar(ts,curr_bt);

			// .onset_allowed_at(beat-number) and .onset_allowed_at(d_t,beat-number)
			bool tf_any_nv_onset_allowed = false;
			for (int j=0; j<vdt.size(); ++j) {
				bool tf = aprx_int_gtest((curr_bt-vph[j])/nbeat(ts,vdt[j]));
				if (tf) {
					EXPECT_TRUE(mg.onset_allowed_at(vdt[j],curr_bt));
				} else {
					EXPECT_FALSE(mg.onset_allowed_at(vdt[j],curr_bt));
				}
				tf_any_nv_onset_allowed = tf_any_nv_onset_allowed || tf;
			}
			if (tf_any_nv_onset_allowed) {
				EXPECT_TRUE(mg.onset_allowed_at(curr_bt));
			} else {
				EXPECT_FALSE(mg.onset_allowed_at(curr_bt));
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
		auto allrps_rpp = mg.enumerate();
		std::vector<std::vector<d_t>> allrps_rp {};
		for (int i=0; i<allrps_rpp.size(); ++i) {
			allrps_rp.push_back(allrps_rpp[i].rp);
		}
		auto allrps_ans = tsts[t].ans_all_rps; 
		EXPECT_TRUE(allrps_rp.size() == allrps_ans.size());
		for (int i=0; i<allrps_ans.size(); ++i) {
			EXPECT_TRUE(std::find(allrps_rp.begin(),allrps_rp.end(),allrps_ans[i]) != allrps_rp.end());
		}

		// draw() should always pull from the allrps_ans set
		for (int i=0; i<tsts[t].niter_draw; ++i) {
			auto curr_rrp = mg.draw();
			EXPECT_TRUE(std::find(allrps_ans.begin(),allrps_ans.end(),curr_rrp) != allrps_ans.end());
		}
	} // To next set in vector tsts
}




