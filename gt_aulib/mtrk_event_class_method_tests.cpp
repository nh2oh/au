#include "gtest/gtest.h"
#include "midi_raw_test_data.h"
#include "..\aulib\input\midi\midi_raw.h"
#include "..\aulib\input\midi\mtrk_event_t.h"
#include <vector>
#include <cstdint>


// 
// Call set_delta_time() on default-constructed object (type()==invalid,...)
//
TEST(mtrk_event_class_method_tests, setDeltaTimeOnDefaultCtordEvent) {
	for (const auto& tc : dt_test_set_a) {
		auto curr_dt_input = static_cast<uint32_t>(tc.dt_input);
		auto curr_dt_ans = static_cast<uint32_t>(tc.ans_value);
		auto curr_dt_sz = tc.ans_n_bytes;

		auto curr_ev = mtrk_event_t();
		curr_ev.set_delta_time(tc.dt_input);

		EXPECT_EQ(curr_ev.delta_time(),curr_dt_ans);
		auto t = curr_ev.type();
		EXPECT_EQ(t,smf_event_type::channel);
		EXPECT_EQ(curr_ev.size(),curr_dt_sz+3);
		EXPECT_EQ(curr_ev.data_size(),3);
		EXPECT_EQ(curr_ev.status_byte(),0x90u);
		EXPECT_EQ(curr_ev.running_status(),0x90u);
	}
}

