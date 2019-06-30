#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include "..\aulib\input\midi\mtrk_event_t.h"
#include <vector>
#include <cstdint>


// 
// Call set_delta_time() on default-constructed object (type()==invalid,...)
//
TEST(mtrk_event_class_method_tests, setDeltaTimeOnDefaultCtordEvent) {
	std::vector<uint32_t> dt_vals {
		0x00u,0x40u,0x7Fu,  // field size == 1
		0x80u,0x2000u,0x3FFFu,  // field size == 2
		0x4000u,0x100000u,0x1FFFFFu,  // field size == 3
		0x00200000u,0x08000000u,0x0FFFFFFFu,  // field size == 4
		// Attempt to write values exceeding the allowed max; field size
		// should be 4, and all values written should be == 0x0FFFFFFFu
		0x1FFFFFFFu,0x2FFFFFFFu,0x7FFFFFFFu,0x8FFFFFFFu,
		0x9FFFFFFFu,0xBFFFFFFFu,0xEFFFFFFFu,0xFFFFFFFFu
	};
	for (const auto& e : dt_vals) {
		auto curr_dt_ans = (0x0FFFFFFFu&e);
		auto curr_dt_sz = 1;
		if ((e>=0x00u) && (e < 0x80u)) {
			curr_dt_sz = 1;
		} else if ((e>= 0x80u) && (e<0x4000u)) {
			curr_dt_sz = 2;
		} else if ((e>= 0x4000u) && (e<0x00200000u)) {
			curr_dt_sz = 3;
		} else {
			curr_dt_sz = 4;
		}

		auto curr_ev = mtrk_event_t();
		curr_ev.set_delta_time(e);
		mtrk_event_unit_test_helper_t h(curr_ev);

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


// 
// Call set_delta_time() _twice_ on default-constructed object 
// (type()==invalid,...).
// Evaluates the ability of set_delta_time() to work with an object that
// is invalid as an MTrk event, but has a valid delta time field.  
//
TEST(mtrk_event_class_method_tests, setDeltaTimeOnInvalidEvent) {
	std::vector<uint32_t> dts {
		0x00u,0x40u,0x7Fu,  // field size == 1
		0x80u,0x2000u,0x3FFFu,  // field size == 2
		0x4000u,0x100000u,0x1FFFFFu,  // field size == 3
		0x00200000u,0x08000000u,0x0FFFFFFFu,  // field size == 4
		// Attempt to write values exceeding the allowed max; field size
		// should be 4, and all values written should be == 0x0FFFFFFFu
		0x1FFFFFFFu,0x2FFFFFFFu,0x7FFFFFFFu,0x8FFFFFFFu,
		0x9FFFFFFFu,0xBFFFFFFFu,0xEFFFFFFFu,0xFFFFFFFFu
	};
	for (const auto& dt_init : dts) {
		auto curr_ev = mtrk_event_t();
		curr_ev.set_delta_time(dt_init);
		mtrk_event_unit_test_helper_t h(curr_ev);
		for (const auto& new_dt : dts) {
			curr_ev.set_delta_time(new_dt);
			auto curr_dt_ans = (0x0FFFFFFFu&new_dt);
			auto curr_dt_sz = 1;
			if ((new_dt>=0x00u) && (new_dt < 0x80u)) {
				curr_dt_sz = 1;
			} else if ((new_dt>= 0x80u) && (new_dt<0x4000u)) {
				curr_dt_sz = 2;
			} else if ((new_dt>= 0x4000u) && (new_dt<0x00200000u)) {
				curr_dt_sz = 3;
			} else {
				curr_dt_sz = 4;
			}

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



