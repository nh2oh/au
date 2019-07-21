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
	for (const auto& e : dt_test_set_a) {
		auto curr_dt_input = static_cast<uint32_t>(e.dt_input);
		auto curr_dt_ans = static_cast<uint32_t>(e.ans_value);
		auto curr_dt_sz = e.ans_n_bytes;

		auto curr_ev = mtrk_event_t();
		curr_ev.set_delta_time(e.dt_input);
		mtrk_event_unit_test_helper_t h(curr_ev);

		EXPECT_EQ(curr_ev.delta_time(),curr_dt_ans);
		EXPECT_EQ(curr_ev.type(),smf_event_type::invalid);
		EXPECT_TRUE(h.is_small());
		EXPECT_FALSE(h.is_big());
		EXPECT_EQ(curr_ev.size(),curr_dt_sz);
		EXPECT_EQ(curr_ev.event_size(),curr_dt_sz);
		EXPECT_EQ(curr_ev.data_size(),0);
		EXPECT_EQ(curr_ev.capacity(),23);
		EXPECT_EQ(curr_ev.status_byte(),0x00u);
		EXPECT_EQ(curr_ev.running_status(),0x00u);
		EXPECT_FALSE(curr_ev.verify());
	}
}


// 
// Call set_delta_time() _twice_ on default-constructed object 
// (type()==invalid,...).
// Evaluates the ability of set_delta_time() to work with an object that
// is invalid as an MTrk event, but has a valid delta time field.  
//
TEST(mtrk_event_class_method_tests, setDeltaTimeOnInvalidEvent) {
	for (const auto& dt_init : dt_test_set_a) {
		auto dt_init_input = static_cast<uint32_t>(dt_init.dt_input);
		auto curr_ev = mtrk_event_t();
		curr_ev.set_delta_time(dt_init_input);
		mtrk_event_unit_test_helper_t h(curr_ev);
		for (const auto& new_dt : dt_test_set_a) {
			auto new_dt_input = static_cast<uint32_t>(new_dt.dt_input);
			auto curr_dt_ans = static_cast<uint32_t>(new_dt.ans_value);
			auto curr_dt_sz = new_dt.ans_n_bytes;

			curr_ev.set_delta_time(new_dt_input);

			EXPECT_EQ(curr_ev.delta_time(),curr_dt_ans);
			EXPECT_EQ(curr_ev.type(),smf_event_type::invalid);
			EXPECT_TRUE(h.is_small());
			EXPECT_FALSE(h.is_big());
			EXPECT_EQ(curr_ev.size(),curr_dt_sz);
			EXPECT_EQ(curr_ev.data_size(),0);
			EXPECT_EQ(curr_ev.capacity(),23);
			EXPECT_EQ(curr_ev.status_byte(),0x00u);
			EXPECT_EQ(curr_ev.running_status(),0x00u);
			EXPECT_FALSE(curr_ev.verify());
		}
	}
}



