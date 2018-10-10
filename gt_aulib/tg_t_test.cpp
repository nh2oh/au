#include "gtest/gtest.h"
#include "..\aulib\util\au_algs_math.h"
#include "..\aulib\types\teejee.h"
#include <vector>


// No phase shift
TEST(tg_t_tests, ThreeFourZeroPhaseHQE) {
	ts_t ts {3_bt,d::q};
	std::vector<d_t> vdt {d::w,d::h,d::q,d::e};
	std::vector<beat_t> ph(vdt.size(),beat_t{0});
	teejee tg {ts,vdt,ph};

	beat_t ans_btres {0.5}; bar_t ans_period {4};
	
	EXPECT_TRUE(tg.validate());
	EXPECT_TRUE(ans_btres == tg.gres());
	EXPECT_TRUE(ans_period == tg.period());
	EXPECT_TRUE(ts == tg.ts());

	int N = 100;
	beat_t btres_test_step {0.25};
	for (int i=0; i<N; ++i) {
		auto curr_nbts = i*btres_test_step;

		if (aprx_int(curr_nbts/4_bt)) {  // whole note
			EXPECT_TRUE(tg.onset_allowed_at(curr_nbts));
			EXPECT_TRUE(tg.factors_at(curr_nbts));
			EXPECT_TRUE(tg.onset_allowed_at({d::w,d::z},curr_nbts));
			EXPECT_TRUE(tg.spans_bar(curr_nbts,d_t{d::w})) 
				<< " at " << curr_nbts.print() + " beats.  ";
				// the w-nt always spans
		} else {
			EXPECT_FALSE(tg.factors_at(curr_nbts));
			EXPECT_FALSE(tg.onset_allowed_at({d::w,d::z},curr_nbts));
		}

		if (aprx_int(curr_nbts/2_bt)) {  // half note
			EXPECT_TRUE(tg.onset_allowed_at(curr_nbts)) 
				<< " at " << curr_nbts.print() + " beats.  ";
			EXPECT_TRUE(tg.onset_allowed_at({d::h,d::z},curr_nbts)) 
				<< " at " << curr_nbts.print() + " beats.  ";
			// Every other h-nt spans the bar
			auto prev_bar_num = std::floor(curr_nbts/3_bt);
			auto next_bar_num = prev_bar_num+1;
			if (next_bar_num*3_bt- curr_nbts < 2_bt) {
				EXPECT_TRUE(tg.spans_bar(curr_nbts,d_t{d::h})) 
					<< " at " << curr_nbts.print() + " beats.  ";
			} else {
				EXPECT_FALSE(tg.spans_bar(curr_nbts,d_t{d::h})) 
					<< " at " << curr_nbts.print() + " beats.  ";
			}
		} else {
			EXPECT_FALSE(tg.factors_at(curr_nbts)) 
				<< " at " << curr_nbts.print() + " beats.  ";
			EXPECT_FALSE(tg.onset_allowed_at({d::h,d::z},curr_nbts)) 
				<< " at " << curr_nbts.print() + " beats.  ";;
		}

		if (aprx_int(curr_nbts/0.5_bt)) {  // 8 note
			EXPECT_TRUE(tg.onset_allowed_at(curr_nbts));
			EXPECT_TRUE(tg.onset_allowed_at({d::e,d::z},curr_nbts));
			EXPECT_FALSE(tg.spans_bar(curr_nbts,d_t{d::e}));  // the 8-note never spans
		} else {
			EXPECT_FALSE(tg.factors_at(curr_nbts)) 
				<< " at " << curr_nbts.print() + " beats.  ";
			EXPECT_FALSE(tg.onset_allowed_at(curr_nbts)) 
				<< " at " << curr_nbts.print() + " beats.  ";
			EXPECT_FALSE(tg.onset_allowed_at({d::e,d::z},curr_nbts)) 
				<< " at " << curr_nbts.print() + " beats.  ";
		}
	}
}



