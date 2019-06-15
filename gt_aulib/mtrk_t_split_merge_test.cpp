#include "gtest/gtest.h"
#include "mtrk_test_data.h"
#include "..\aulib\input\midi\mtrk_t.h"
#include "..\aulib\input\midi\mtrk_event_t.h"
#include <vector>
#include <cstdint>
#include <iterator>

using namespace mtrk_tests;

mtrk_t make_tsb() {
	auto mtrk_tsb = mtrk_t();  // auto to avoid MVP
	for (const auto& e : tsb) {
		auto curr_ev = mtrk_event_t(e.d.data(),e.d.size());
		mtrk_tsb.push_back(curr_ev);
	}
	return mtrk_tsb;
}


// 
// OIt split_copy_if(InIt beg, InIt end, OIt dest, UPred pred)
//
TEST(mtrk_t_tests, SplitCopyIfForNoteNum67WithTSB) {
	auto mtrk_b = make_tsb();

	auto isntnum43 = [](const mtrk_event_t& ev)->bool {
		auto md = get_channel_event(ev);
		return (is_channel_voice(ev) && (md.p1==67));  // 67 == 0x43u
	};

	auto new_mtrk = mtrk_t();
	auto bi = std::back_inserter(new_mtrk);

	auto it = split_copy_if(mtrk_b.begin(),mtrk_b.end(),
		bi,isntnum43);

	for (const auto& e : new_mtrk) {
		EXPECT_TRUE(is_channel_voice(e));
		auto md = get_channel_event(e);
		EXPECT_EQ(md.p1,67);
	}

}


