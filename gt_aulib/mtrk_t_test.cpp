#include "gtest/gtest.h"
#include "mtrk_test_data.h"
#include "..\aulib\input\midi\mtrk_t.h"
#include "..\aulib\input\midi\mtrk_event_t.h"
#include <vector>
#include <cstdint>

using namespace mtrk_tests;

// 
// 
//
TEST(mtrk_t_tests, InsertEventsWithZeroDtIntoTestSetA) {
	auto mtrk_tsa = mtrk_t();  // auto to avoid MVP
	for (const auto& e : tsa) {
		auto curr_ev = mtrk_event_t(e.d.data(),e.d.size());
		mtrk_tsa.push_back(curr_ev);
	}
	// On, off events for note num 57 on ch=0; both events have delta-time
	// == 0.  
	mtrk_event_t e_on(0,midi_ch_event_t {0x90u,0,57,25});
	mtrk_event_t e_off(0,midi_ch_event_t {0x90u,0,57,0});
	std::vector<int> idxs {3,4,5,6,7,8,9,10};
	for (const auto i : idxs) {
		auto curr_ev = mtrk_event_t(tsa[i].d.data(),tsa[i].d.size());
		EXPECT_EQ(mrk_tsa[i],curr_ev);
	}


}
