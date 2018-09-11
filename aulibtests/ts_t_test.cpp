#include "pch.h"
#include "..\aulib\types\ts_t.h"
#include <vector>


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

