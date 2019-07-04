#include "gtest/gtest.h"
#include "..\aulib\input\midi\mtrk_event_t.h"
#include "..\aulib\input\midi\mtrk_event_methods.h"
#include "sysex_factory_test_data.h"
#include <vector>
#include <cstdint>


// 
// mtrk_event_t make_sysex_f0(const uint32_t& dt, 
//								std::vector<unsigned char> payload);
//
// Input payloads that lack a terminating 0xF7u; Expect that the factory
// func will add the 0xF7u.  
//
TEST(mtrk_event_sysex_factories, makeSysexF0PayloadsLackTerminalF7) {
	for (const auto& e : f0_tests_no_terminating_f7_on_pyld) {
		auto curr_dtN = delta_time_field_size(e.ans_dt);
		auto ans_event_size = 1 + midi_vl_field_size(e.ans_pyld_len) 
			+ e.ans_pyld_len;
		auto ans_tot_size = curr_dtN + ans_event_size;
		bool ans_is_small = (ans_tot_size<=23);
		auto ans_payload = e.payload_in;  ans_payload.push_back(0xF7u);

		auto ev = make_sysex_f0(e.dt_in,e.payload_in);
		mtrk_event_unit_test_helper_t h (ev);

		if (ans_is_small) {
			EXPECT_TRUE(h.is_small());
			EXPECT_FALSE(h.is_big());
		} else {
			EXPECT_TRUE(h.is_big());
			EXPECT_FALSE(h.is_small());
		}
		EXPECT_EQ(ev.type(),smf_event_type::sysex_f0);
		EXPECT_TRUE(is_sysex(ev));
		EXPECT_TRUE(is_sysex_f0(ev));
		EXPECT_FALSE(is_sysex_f7(ev));

		EXPECT_EQ(ev.delta_time(),e.ans_dt);
		EXPECT_EQ(ev.size(),ans_tot_size);
		EXPECT_EQ(ev.data_size(),ans_event_size);
		EXPECT_TRUE(ev.capacity() >= ev.size());
		EXPECT_EQ(ev.running_status(),0x00u);
		EXPECT_EQ(ev.status_byte(),0xF0u);

		EXPECT_EQ(ev.begin(),ev.dt_begin());
		EXPECT_EQ(ev.event_begin(),ev.dt_end());
		EXPECT_EQ((ev.end()-ev.begin()),ev.size());
		EXPECT_EQ((ev.end()-ev.dt_begin()),ev.size());
		EXPECT_EQ((ev.end()-ev.event_begin()),ans_event_size);
		EXPECT_EQ((ev.end()-ev.payload_begin()),e.ans_pyld_len);
		
		ASSERT_EQ(ans_payload.size(), (ev.end()-ev.payload_begin()));
		auto it = ev.payload_begin();
		for (int i=0; i<ans_payload.size(); ++i) {
			EXPECT_EQ(*it++,ans_payload[i]);
		}
	}
}


