#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include "..\aulib\input\midi\mtrk_container_t.h"
#include <vector>
#include <array>
#include <cstdint>


// 
// "Small" (in the SBO-sense) meta events from the midi std and from files
// observed in the wild.  
//
TEST(mtrk_event_container_sbo_tests, metaEventsSmall) {
	
	struct test_ans_t {
		bool is_small {true};
		uint32_t delta_time {0};
		//smf_event_type type {smf_event_type::meta};
		uint32_t size {0};
		uint32_t data_size {0};
	};
	struct p142tests_t {
		std::vector<unsigned char> bytes {};
		test_ans_t ans {};
	};

	std::vector<p142tests_t> tests {
		//
		// From p.142 of the midi std
		//
		{{0x00,0xFF,0x58,0x04,0x04,0x02,0x18,0x08},  // Time sig
		{true,0x00,8,7}},

		{{0x00,0xFF,0x51,0x03,0x07,0xA1,0x20},  // Tempo
		{true,0x00,7,6}},

		{{0x00,0xFF,0x2F,0x00},  // End of track
		{true,0x00,4,3}},

		//
		// Not from the midi std:
		//
		{{0x00,0xFF,0x51,0x03,0x07,0xA1,0x20},  // Tempo (CLEMENTI.MID)
		{true,0x00,7,6}},

		// Padded w/ zeros, but otherwise identical to above
		{{0x00,0xFF,0x51,0x03,0x07,0xA1,0x20,0x00,0x00,0x00,0x00},  // Tempo (CLEMENTI.MID)
		{true,0x00,7,6}},

		{{0x00,0xFF,0x01,0x10,0x48,0x61,0x72,0x70,0x73,0x69,0x63,0x68,
		0x6F,0x72,0x64,0x20,0x48,0x69,0x67,0x68},  // Text element "Harpsichord High"
		{true,0x00,20,19}}
	};
	
	for (const auto& e : tests) {
		mtrk_event_container_sbo_t c(e.bytes.data(),e.bytes.size(),0);
		//EXPECT_EQ(c.type(),e.ans.type);
		EXPECT_EQ(c.type(),smf_event_type::meta);
		EXPECT_EQ(c.delta_time(),e.ans.delta_time);
		
		EXPECT_TRUE(c.is_small());
		EXPECT_FALSE(c.is_big());
		EXPECT_EQ(c.size(),e.ans.size);
		EXPECT_EQ(c.data_size(),e.ans.data_size);
		for (int i=0; i<e.ans.size; ++i) {
			EXPECT_EQ(c[i],e.bytes[i]);
			EXPECT_EQ(*(c.data()+i),e.bytes[i]);
		}
	}
	
}

