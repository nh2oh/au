#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include "..\aulib\input\midi\mtrk_container_t.h"
#include <vector>
#include <cstdint>


// 
// "Small" (in the SBO-sense) meta events from the midi std and from files
// observed in the wild.  
//
// Tests of the mtrk_event_container_sbo_t(unsigned char *, int, unsigned char) ctor
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


TEST(mtrk_event_container_sbo_tests, metaEventsSmallCopyCtorAndCopyAssign) {
	std::vector<std::vector<unsigned char>> tests {
		//
		// From p.142 of the midi std
		//
		{{0x00,0xFF,0x58,0x04,0x04,0x02,0x18,0x08}},  // Time sig
		{{0x00,0xFF,0x51,0x03,0x07,0xA1,0x20}},  // Tempo
		{{0x00,0xFF,0x2F,0x00}},  // End of track
		//
		// Not from the midi std
		//
		{{0x00,0xFF,0x51,0x03,0x07,0xA1,0x20}},  // Tempo (CLEMENTI.MID)
		// Padded w/ zeros, but otherwise identical to above:
		{{0x00,0xFF,0x51,0x03,0x07,0xA1,0x20,0x00,0x00,0x00,0x00}},
		{{0x00,0xFF,0x01,0x10,0x48,0x61,0x72,0x70,0x73,0x69,0x63,0x68,
			0x6F,0x72,0x64,0x20,0x48,0x69,0x67,0x68}}   // Text element "Harpsichord High"
	};

	for (const auto& e : tests) {
		mtrk_event_container_sbo_t c1(e.data(),e.size(),0);
		
		//---------------------------------------------------------------------------
		// copy ctor:
		auto c2 = c1;

		//EXPECT_EQ(c2.type(),c1.type());
		EXPECT_EQ(c2.type(),c1.type());
		EXPECT_EQ(c2.delta_time(),c1.delta_time());

		EXPECT_TRUE(c2.is_small());
		EXPECT_FALSE(c2.is_big());
		EXPECT_EQ(c2.size(),c1.size());
		EXPECT_EQ(c2.data_size(),c1.data_size());
		for (int i=0; i<e.size(); ++i) {
			EXPECT_EQ(c2[i],e[i]);
			EXPECT_EQ(*(c2.data()+i),e[i]);
		}

		//---------------------------------------------------------------------------
		// copy assign:
		mtrk_event_container_sbo_t c3(tests[0].data(),tests[0].size(),0);
		c3 = c1;

		//EXPECT_EQ(c3.type(),c1.type());
		EXPECT_EQ(c3.type(),c1.type());
		EXPECT_EQ(c3.delta_time(),c1.delta_time());

		EXPECT_TRUE(c3.is_small());
		EXPECT_FALSE(c3.is_big());
		EXPECT_EQ(c3.size(),c1.size());
		EXPECT_EQ(c3.data_size(),c1.data_size());
		for (int i=0; i<e.size(); ++i) {
			EXPECT_EQ(c3[i],e[i]);
			EXPECT_EQ(*(c3.data()+i),e[i]);
		}
	}

}



// 
// "Big" (in the SBO-sense) meta events from the midi std and from files
// observed in the wild.  
//
// Tests of the mtrk_event_container_sbo_t(unsigned char *, int, unsigned char) ctor
//
TEST(mtrk_event_container_sbo_tests, metaEventsBig) {
	struct test_ans_t {
		bool is_small {true};
		uint32_t delta_time {0};
		//smf_event_type type {smf_event_type::meta};
		uint32_t size {0};
		uint32_t data_size {0};
	};
	struct tests_t {
		std::vector<unsigned char> bytes {};
		test_ans_t ans {};
	};

	std::vector<tests_t> tests {
	//
	// Not from the midi std:
	//
		{{0x00,0xFF,0x03,0x1D,0x48,0x61,0x6C,0x6C,0x65,0x6C,0x75,0x6A,
			0x61,0x68,0x21,0x20,0x4A,0x6F,0x79,0x20,0x74,0x6F,0x20,0x74,
			0x68,0x65,0x20,0x57,0x6F,0x72,0x6C,0x64,0x21},  // Tempo (CLEMENTI.MID)
		{false,0x00,33,32}}
	};

	for (const auto& e : tests) {
		mtrk_event_container_sbo_t c(e.bytes.data(),e.bytes.size(),0);
		//EXPECT_EQ(c.type(),e.ans.type);
		EXPECT_EQ(c.type(),smf_event_type::meta);
		EXPECT_EQ(c.delta_time(),e.ans.delta_time);

		EXPECT_FALSE(c.is_small());
		EXPECT_TRUE(c.is_big());
		EXPECT_EQ(c.size(),e.ans.size);
		EXPECT_EQ(c.data_size(),e.ans.data_size);
		for (int i=0; i<e.ans.size; ++i) {
			EXPECT_EQ(c[i],e.bytes[i]);
			EXPECT_EQ(*(c.data()+i),e.bytes[i]);
		}
	}

}

