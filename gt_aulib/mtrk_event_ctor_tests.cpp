#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include "..\aulib\input\midi\midi_vlq.h"
#include "..\aulib\input\midi\mtrk_event_t.h"
#include "..\aulib\input\midi\mtrk_event_methods.h"
#include <vector>
#include <cstdint>


// 
// Test of the default-constructed value, a 1-sized small-capacity event
// w/a 0 delta-time.  Although the event array is all 0x00u's, size()==1
// (not zero), b/c the initial 0x00u is interpreted as a 1-byte delta-time.  
//
//// TODO:  Solve mtrk_event_t_internal::small_capacity() linker problem
//
TEST(mtrk_event_ctor_tests, defaultConstructedValue) {
	auto default_event = mtrk_event_t();
	mtrk_event_unit_test_helper_t h(default_event);

	EXPECT_EQ(default_event.delta_time(),0);
	EXPECT_EQ(default_event.type(),smf_event_type::invalid);
	EXPECT_TRUE(h.is_small());
	EXPECT_FALSE(h.is_big());
	EXPECT_EQ(default_event.size(),1);  
	EXPECT_EQ(default_event.data_size(),0);
	EXPECT_EQ(default_event.capacity(),23);
	EXPECT_EQ(default_event.status_byte(),0x00u);
	EXPECT_EQ(default_event.running_status(),0x00u);
	EXPECT_FALSE(default_event.verify());
}

// 
// Test of the mtrk_event_t(uint32_t dt) ctor, which constructs a dt-sized 
// small-capacity event.  Although the event array beyond the delta-time is
// all 0x00u's.  If the delta-time value provided to the ctor exceeds the max
// value for a 4-byte vlq (0x0FFFFFFFu), delta_time is set to this maximum.  
//
// TODO:  Solve mtrk_event_t_internal::small_capacity() linker problem
//
TEST(mtrk_event_ctor_tests, dtOnlyCtor) {
	std::vector<uint32_t> tests {
		0x00u,0x40u,0x7Fu,  // field size == 1
		0x80u,0x2000u,0x3FFFu,  // field size == 2
		0x4000u,0x100000u,0x1FFFFFu,  // field size == 3
		0x00200000u,0x08000000u,0x0FFFFFFFu,  // field size == 4
		// Attempt to write values exceeding the allowed max; field size
		// should be 4, and all values written should be == 0x0FFFFFFFu
		0x1FFFFFFFu,0x2FFFFFFFu,0x7FFFFFFFu,0x8FFFFFFFu,
		0x9FFFFFFFu,0xBFFFFFFFu,0xEFFFFFFFu,0xFFFFFFFFu
	};
	for (const auto& e : tests) {
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

		mtrk_event_t curr_ev(e);
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
// Tests of the mtrk_event_t(unsigned char *, int, unsigned char)
// ctor with a set of "small" (in the SBO-sense) midi_channel,meta,sysex_f0/f7
// events from the midi std and from files observed in the wild.  
//
// All events are constructed passing 0 for the running-status
//
TEST(mtrk_event_ctor_tests, assortedSMFEventsSmallZeroRS) {
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
		// From p.142 of the midi std
		{{0x00,0xFF,0x58,0x04,0x04,0x02,0x18,0x08},  // meta, Time sig
		{true,0x00,8,7}},
		{{0x00,0xFF,0x51,0x03,0x07,0xA1,0x20},  // meta, Tempo
		{true,0x00,7,6}},
		{{0x00,0xFF,0x2F,0x00},  // meta, End of track
		{true,0x00,4,3}},

		// From p. 136 of the midi std:
		{{0x00,0xF0,0x03,0x43,0x12,0x00},  // sysex_f0
		{true,0x00,6,5}},
		{{0x81,0x48,0xF7,0x06,0x43,0x12,0x00,0x43,0x12,0x00},  // sysex_f7
		{true,200,10,8}},
		{{0x64,0xF7,0x04,0x43,0x12,0x00,0xF7},  // sysex_f7
		{true,100,7,6}},

		// From p. 141 of the midi std:
		{{0x00,0x92,0x48,0x96},  // Ch. 3 Note On #48, forte; dt=0
		{true,0x00,4,3}},
		{{0x81,0x48,0x82,0x48,0x64},  // Ch. 3 Note Off #48, standard; dt=200
		{true,200,5,3}},
	};

	for (const auto& e : tests) {
		mtrk_event_t c2(e.bytes.data(),e.bytes.size(),0);
		//EXPECT_EQ(c.type(),e.ans.type);  // TODO:  Add test when vs fixes its enum class issues
		EXPECT_EQ(c2.delta_time(),e.ans.delta_time);

		mtrk_event_unit_test_helper_t h(c2);
		EXPECT_TRUE(h.is_small());
		EXPECT_FALSE(h.is_big());
		EXPECT_EQ(c2.size(),e.ans.size);
		EXPECT_EQ(c2.data_size(),e.ans.data_size);
		for (int i=0; i<e.ans.size; ++i) {
			EXPECT_EQ(c2[i],e.bytes[i]);
			EXPECT_EQ(*(c2.data()+i),e.bytes[i]);
		}
	}
}

//
// Tests of the mtrk_event_t(unsigned char *, int, unsigned char)
// ctor
// 
// "Small" (in the SBO-sense) midi events from a random set i generated
// on 04/21/19.  All test cases are valid midi events.  Some are in running-
// status, others not.  The delta-times are random.  
// Values in tests_t.midisb_prev_event are meant to test the ability of
// the ctor to use the running-status when needed to parse the event.  
// Some of the MIDI events have no event-local status-byte, in which case the
// running-status byte supplied w/ the test case (tests_t.midisb_prev_event) 
// is valid and accurately describes the number of data bytes in the message.
// Events that _do_ have a valid event-local status byte are sometimes 
// coupled with a valid running-status, and sometimes with an invalid 
// running-status.  
//
TEST(mtrk_event_ctor_tests, randomSMFEvntsSmallRandomRS) {
	// Fields applic_midi_status, n_data_bytes are not (yet) tested here
	struct tests_t {
		std::vector<unsigned char> data {};
		unsigned char midisb_prev_event {};  // "midi status byte previous event"
		unsigned char applic_midi_status {};  // "applicable midi status byte"
		uint8_t n_data_bytes {};  // Based on value of applic_midi_status
		uint32_t data_length {};  // n_data_bytes (+ 1 if not in running status)
		uint32_t dt_value {};
		uint8_t dt_field_size {};
	};
	std::vector<tests_t> tests {
		{{0xC0,0x80,0x00,0x01},0xD0,0xD0,1,1,1048576,3},
		{{0x81,0x80,0x80,0x00,0xC1,0x00},0xC0,0xC1,1,2,2097152,4},
		{{0xC0,0x00,0x09},0xC0,0xC0,1,1,8192,2},
		{{0xC0,0x80,0x80,0x00,0xC1,0x70},0x8A,0xC1,1,2,134217728,4},
		{{0x7F,0x00,0x0F},0x81,0x81,2,2,127,1},
		{{0x00,0xD1,0x0F},0xD1,0xD1,1,2,0,1},
		{{0xFF,0xFF,0xFF,0x7F,0x77,0x70},0x80,0x80,2,2,268435455,4},
		{{0x81,0x00,0x10},0xDF,0xDF,1,1,128,2},
		{{0xFF,0xFF,0x7F,0x90,0x09,0x0A},0xC0,0x90,2,3,2097151,3},
		{{0xFF,0x7F,0xC0,0x09},0xD1,0xC0,1,2,16383,2},
		{{0xFF,0xFF,0x7F,0xC0,0x0F},0x90,0xC0,1,2,2097151,3},
		{{0x81,0x48,0xE1,0x77,0x03},0xDF,0xE1,2,3,200,2},
		{{0x81,0x48,0x01,0x02},0x9F,0x9F,2,2,200,2},
		{{0x81,0x80,0x00,0x70},0xCB,0xCB,1,1,16384,3},
		{{0x81,0x80,0x00,0x01,0x01},0xB1,0xB1,2,2,16384,3},
		{{0x81,0x00,0x01,0x03},0x90,0x90,2,2,128,2},
		{{0xC0,0x80,0x00,0x9B,0x02,0x0F},0x91,0x9B,2,3,1048576,3},
		{{0x81,0x80,0x00,0x10},0xDB,0xDB,1,1,16384,3},
		{{0xC0,0x80,0x80,0x00,0x8F,0x03,0x01},0x81,0x8F,2,3,134217728,4},
		{{0xFF,0x7F,0xEB,0x77,0x0A},0x7F,0xEB,2,3,16383,2},
		{{0x7F,0x09},0xC1,0xC1,1,1,127,1},
		{{0x81,0x00,0x80,0x03,0x03},0xEB,0x80,2,3,128,2},
		{{0xC0,0x00,0x0A,0x0A},0x91,0x91,2,2,8192,2},
		{{0x81,0x48,0xCB,0x0F},0xD0,0xCB,1,2,200,2},
		{{0xFF,0x7F,0x90,0x70,0x0F},0xDF,0x90,2,3,16383,2},
		{{0x81,0x80,0x00,0xDB,0x10},0xEB,0xDB,1,2,16384,3},
		{{0x81,0x48,0x01,0x03},0xEB,0xEB,2,2,200,2},
		{{0x81,0x48,0x02},0xD1,0xD1,1,1,200,2},
		{{0xFF,0x7F,0x90,0x0A,0x10},0xDF,0x90,2,3,16383,2},
		{{0x40,0x01,0x00},0xE0,0xE0,2,2,64,1},
		{{0xC0,0x80,0x00,0x77,0x10},0x9F,0x9F,2,2,1048576,3},
		{{0x81,0x80,0x80,0x00,0x90,0x09,0x0F},0xB1,0x90,2,3,2097152,4},
		{{0xFF,0x7F,0xCB,0x0F},0xEB,0xCB,1,2,16383,2},
		{{0x81,0x80,0x80,0x00,0x02},0xDF,0xDF,1,1,2097152,4},
		{{0x81,0x48,0xDB,0x77},0xC0,0xDB,1,2,200,2},
		{{0x81,0x48,0xDB,0x00},0xBF,0xDB,1,2,200,2},
		{{0x81,0x48,0xEF,0x0F,0x0F},0x90,0xEF,2,3,200,2},
		{{0xFF,0xFF,0xFF,0x7F,0x03,0x01},0xEB,0xEB,2,2,268435455,4},
		{{0xFF,0xFF,0xFF,0x7F,0x77,0x10},0x8A,0x8A,2,2,268435455,4},
		{{0xC0,0x00,0xC1,0x00},0xE0,0xC1,1,2,8192,2},
		{{0xC0,0x80,0x00,0xC1,0x70},0x80,0xC1,1,2,1048576,3},
		{{0x7F,0x77},0xCF,0xCF,1,1,127,1},
		{{0x81,0x80,0x80,0x00,0x02,0x02},0x90,0x90,2,2,2097152,4},
		{{0x81,0x00,0xC1,0x70},0xB0,0xC1,1,2,128,2},
		{{0xC0,0x80,0x80,0x00,0xD0,0x10},0xE0,0xD0,1,2,134217728,4},
		{{0x81,0x80,0x80,0x00,0xC0,0x10},0xEB,0xC0,1,2,2097152,4},
		{{0x64,0x70,0x03},0xE1,0xE1,2,2,100,1},
		{{0x81,0x80,0x80,0x00,0x80,0x0A,0x77},0xF0,0x80,2,3,2097152,4},
		{{0x81,0x80,0x00,0x01,0x0F},0x8F,0x8F,2,2,16384,3},
		{{0x81,0x80,0x00,0x02,0x0A},0xEF,0xEF,2,2,16384,3},
		{{0x81,0x80,0x80,0x00,0x02},0xDF,0xDF,1,1,2097152,4},
		{{0x7F,0x0F,0x70},0xE0,0xE0,2,2,127,1},
		{{0x7F,0x00},0xDF,0xDF,1,1,127,1},
		{{0x7F,0x01,0x70},0x9B,0x9B,2,2,127,1},
		{{0x7F,0x01},0xDB,0xDB,1,1,127,1},
		{{0xFF,0x7F,0x0A},0xCF,0xCF,1,1,16383,2},
		{{0x81,0x48,0xD0,0x01},0x7F,0xD0,1,2,200,2},
		{{0xC0,0x00,0xCF,0x77},0x8A,0xCF,1,2,8192,2},
		{{0xFF,0xFF,0xFF,0x7F,0x0A,0x02},0xEF,0xEF,2,2,268435455,4},
		{{0x81,0x00,0xDF,0x02},0xC0,0xDF,1,2,128,2},
		{{0xC0,0x00,0x70,0x09},0xEF,0xEF,2,2,8192,2},
		{{0xC0,0x80,0x80,0x00,0xBB,0x00,0x01},0xCB,0xBB,2,3,134217728,4},
		{{0xC0,0x00,0xCB,0x01},0x9B,0xCB,1,2,8192,2},
		{{0x81,0x00,0x00,0x00},0xE0,0xE0,2,2,128,2},
		{{0xFF,0xFF,0x7F,0x8A,0x01,0x02},0xBF,0x8A,2,3,2097151,3},
		{{0xFF,0xFF,0x7F,0x10,0x77},0x90,0x90,2,2,2097151,3},
		{{0xC0,0x80,0x80,0x00,0xDB,0x03},0x9F,0xDB,1,2,134217728,4},
		{{0x40,0xCF,0x77},0x8F,0xCF,1,2,64,1},
		{{0x7F,0x01},0xD1,0xD1,1,1,127,1},
		{{0xFF,0xFF,0xFF,0x7F,0x0A,0x70},0x80,0x80,2,2,268435455,4},
		{{0xC0,0x00,0x70},0xD1,0xD1,1,1,8192,2},
		{{0x64,0x03,0x77},0xB1,0xB1,2,2,100,1},
		{{0x00,0xCB,0x70},0x81,0xCB,1,2,0,1},
		{{0x64,0xD0,0x0A},0x7F,0xD0,1,2,100,1},
		{{0xC0,0x80,0x00,0x77},0xC0,0xC0,1,1,1048576,3},
		{{0x81,0x80,0x80,0x00,0xE1,0x10,0x09},0xB0,0xE1,2,3,2097152,4},
		{{0x81,0x80,0x80,0x00,0xD0,0x01},0x8A,0xD0,1,2,2097152,4},
		{{0x81,0x80,0x00,0xCF,0x70},0x9F,0xCF,1,2,16384,3},
		{{0x7F,0x9F,0x09,0x10},0xB0,0x9F,2,3,127,1},
		{{0x00,0x90,0x0A,0x02},0x81,0x90,2,3,0,1},
		{{0xFF,0xFF,0x7F,0x0A},0xDF,0xDF,1,1,2097151,3},
		{{0xFF,0xFF,0x7F,0x91,0x00,0x0F},0xB1,0x91,2,3,2097151,3},
		{{0x81,0x48,0x02},0xD0,0xD0,1,1,200,2},
		{{0x81,0x00,0xDB,0x02},0xD0,0xDB,1,2,128,2},
		{{0x7F,0xC1,0x00},0xDF,0xC1,1,2,127,1},
		{{0xC0,0x00,0xB0,0x03,0x0A},0xCB,0xB0,2,3,8192,2},
		{{0x81,0x48,0x10},0xDB,0xDB,1,1,200,2},
		{{0x00,0xDF,0x02},0x8F,0xDF,1,2,0,1},
		{{0x81,0x48,0x70,0x10},0x9F,0x9F,2,2,200,2},
		{{0xC0,0x80,0x80,0x00,0x01},0xD0,0xD0,1,1,134217728,4},
		{{0xFF,0xFF,0x7F,0x0A},0xC1,0xC1,1,1,2097151,3},
		{{0x81,0x80,0x00,0xCB,0x09},0x91,0xCB,1,2,16384,3},
		{{0xFF,0xFF,0x7F,0xEF,0x09,0x09},0xB1,0xEF,2,3,2097151,3},
		{{0xFF,0x7F,0xB1,0x0F,0x00},0xF0,0xB1,2,3,16383,2},
		{{0xC0,0x00,0xB1,0x70,0x0A},0x8F,0xB1,2,3,8192,2},
		{{0x7F,0xB1,0x03,0x10},0x80,0xB1,2,3,127,1},
		{{0x00,0xDF,0x03},0x8F,0xDF,1,2,0,1},
		{{0x81,0x00,0x03},0xCB,0xCB,1,1,128,2},
		{{0x64,0x9B,0x0F,0x03},0xDF,0x9B,2,3,100,1},
		{{0xC0,0x80,0x80,0x00,0x90,0x03,0x0A},0xD1,0x90,2,3,134217728,4},
	};

	for (auto& e : tests) {
		mtrk_event_t c2(e.data.data(),e.data.size(),e.midisb_prev_event);
		//EXPECT_EQ(c.type(),e.ans.type);  // TODO:  Add test when vs fixes its enum class issues
		EXPECT_EQ(c2.delta_time(),e.dt_value);
		mtrk_event_unit_test_helper_t h(c2);

		auto data_size_with_status_byte = e.n_data_bytes + 1;
		bool input_is_in_rs = e.data_length < data_size_with_status_byte;
		auto size_with_status_byte = e.dt_field_size + data_size_with_status_byte;

		EXPECT_TRUE(h.is_small());
		EXPECT_FALSE(h.is_big());
		EXPECT_EQ(c2.size(),size_with_status_byte);  //EXPECT_EQ(c2.size(),e.data.size());
		EXPECT_EQ(c2.data_size(),data_size_with_status_byte);  //EXPECT_EQ(c2.data_size(),e.data_length);
		//EXPECT_TRUE(c2.validate());
		if (input_is_in_rs) {
			e.data.insert(e.data.begin()+e.dt_field_size,e.applic_midi_status);
		}
		for (int i=0; i<c2.size(); ++i) {  //for (int i=0; i<e.data.size(); ++i) {
			EXPECT_EQ(c2[i],e.data[i]);
			EXPECT_EQ(*(c2.data()+i),e.data[i]);
		}
	}
}


//
// Tests of the mtrk_event_t(unsigned char *, int, unsigned char)
// ctor.  
// 
// "Small" (in the SBO-sense) meta events from the midi std and from files
// observed in the wild.  All constructed passing 0 for the value of the 
// running-status.  
//
TEST(mtrk_event_ctor_tests, metaEventsSmallZeroRS) {
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
		mtrk_event_t c2(e.bytes.data(),e.bytes.size(),0);
		mtrk_event_unit_test_helper_t h(c2);
		//EXPECT_EQ(c.type(),e.ans.type);
		EXPECT_EQ(c2.type(),smf_event_type::meta);
		EXPECT_EQ(c2.delta_time(),e.ans.delta_time);
		
		EXPECT_TRUE(h.is_small());
		EXPECT_FALSE(h.is_big());
		EXPECT_EQ(c2.size(),e.bytes.size());
		EXPECT_EQ(c2.event_size(),e.ans.size);
		EXPECT_EQ(c2.data_size(),e.ans.data_size);
		//EXPECT_TRUE(c2.validate());
		for (int i=0; i<e.ans.size; ++i) {
			EXPECT_EQ(c2[i],e.bytes[i]);
			EXPECT_EQ(*(c2.data()+i),e.bytes[i]);
		}
	}
}

//
// Tests of the mtrk_event_t(uint32_t, const midi_ch_event_t&) ctor
// with valid data in the midi_ch_event_t struct.  
//
TEST(mtrk_event_ctor_tests, MidiChEventStructCtorValidInputData) {
	struct test_t {
		uint32_t dt_input {0};
		midi_ch_event_t md_input {};
		uint32_t data_size {0};
	};
	// midi_ch_event_t {status, ch, p1, p2}
	std::vector<test_t> tests {
		// Events w/ 2 data bytes:
		{0, {note_on,0,57,32}, 3},
		{23, {note_off,1,57,32}, 3},
		{12354, {key_pressure,0,57,32}, 3},
		{0, {ctrl_change,15,72,100}, 3},
		{45541, {pitch_bend,0,127,127}, 3},
		// Events w/ 1 data byte:
		{785, {prog_change,14,127,0x00u}, 2},
		{2, {ch_pressure,2,0,0x00u}, 2}
	};
	
	for (const auto& e : tests) {
		unsigned char curr_s = (e.md_input.status_nybble + e.md_input.ch);
		int curr_dt_size = midi_vl_field_size(e.dt_input);
		int curr_size = curr_dt_size+e.data_size;
		mtrk_event_t ev(e.dt_input,e.md_input);
		mtrk_event_unit_test_helper_t h(ev);

		EXPECT_EQ(ev.type(),smf_event_type::channel);
		EXPECT_EQ(ev.delta_time(),e.dt_input);
		EXPECT_TRUE(h.is_small());
		EXPECT_FALSE(h.is_big());
		EXPECT_EQ(ev.size(),curr_size);
		EXPECT_EQ(ev.data_size(),e.data_size);
		EXPECT_EQ(ev.status_byte(),curr_s);
		EXPECT_EQ(ev.running_status(),curr_s);

		auto it_beg = ev.event_begin();
		auto it_end = ev.end();
		EXPECT_EQ((it_end-ev.begin()),curr_size);
		EXPECT_EQ((it_end-it_beg),curr_size-curr_dt_size);
		EXPECT_EQ(it_beg,ev.payload_begin());
		EXPECT_EQ(*it_beg++,curr_s);
		EXPECT_EQ(*it_beg++,e.md_input.p1);
		if (it_beg < it_end) {
			EXPECT_TRUE((it_end-it_beg)==1);
			EXPECT_EQ(*it_beg++,e.md_input.p2);
		}
		EXPECT_TRUE(it_beg==it_end);
		EXPECT_TRUE(it_beg>=it_end);
		EXPECT_TRUE(it_beg<=it_end);
		EXPECT_FALSE(it_beg>it_end);
		EXPECT_FALSE(it_beg<it_end);
	}
}



//
// Tests of the mtrk_event_t(uint32_t, const midi_ch_event_t&) ctor
// with _invalid_ data in the midi_ch_event_t struct.  
//
// TODO:  The data in md_result is not used, since for this ctor the
// 'result' values are UB.  Keep to use this test data for tests of 
// normalize(midi_ch_event_t).  
TEST(mtrk_event_ctor_tests, MidiChEventStructCtorInvalidInputData) {
	struct test_t {
		uint32_t dt_input {0};
		midi_ch_event_t md_input {};
		midi_ch_event_t md_result {};
		uint32_t data_size {0};
	};
	// midi_ch_event_t {status, ch, p1, p2}
	std::vector<test_t> tests {
		{0, {note_on,16,57,32}, {note_on,0,57,32}, 3},  // Invalid channel (>15)
		{1, {note_on,127,57,32}, {note_on,15,57,32}, 3},  // Invalid channel (>15)
		{128, {note_on,14,128,32}, {note_on,14,0,32}, 3},  // Invalid p1
		{256, {note_on,14,129,32}, {note_on,14,1,32}, 3},  // Invalid p1
		{512, {note_on,14,7,130}, {note_on,14,7,2}, 3},  // Invalid p2
		{1024, {note_on,14,57,255}, {note_on,14,57,0x7Fu}, 3},  // Invalid p2

		// Exactly the same as the set above, but w/a 1-data-byte msg type
		{0, {prog_change,16,57,32}, {prog_change,0,57,0x80u}, 2},  // Invalid channel
		{1, {prog_change,127,57,32}, {prog_change,15,57,0x80u}, 2},  // Invalid channel
		{128, {prog_change,14,128,32}, {prog_change,14,0,0x80u}, 2},  // Invalid p1
		{256, {prog_change,14,129,32}, {prog_change,14,1,0x80u}, 2},  // Invalid p1
		{512, {prog_change,14,7,130}, {prog_change,14,7,0x80u}, 2},  // Invalid p2
		{1024, {prog_change,14,57,255}, {prog_change,14,57,0x80u}, 2},  // Invalid p2

		// Exactly the same as the set above, but w/an invalid status-nybble
		{0, {note_on&0x7Fu,16,57,32}, {note_on,0,57,32}, 3},  // Invalid channel
		{1, {note_on&0x7Fu,127,57,32}, {note_on,15,57,32}, 3},  // Invalid channel
		{128, {note_on&0x7Fu,14,128,32}, {note_on,14,0,32}, 3},  // Invalid p1
		{256, {note_on&0x7Fu,14,129,32}, {note_on,14,1,32}, 3},  // Invalid p1
		{512, {note_on&0x7Fu,14,7,130}, {note_on,14,7,2}, 3},  // Invalid p2
		{1024, {note_on&0x7Fu,14,57,255}, {note_on,14,57,0x7Fu}, 3}  // Invalid p2
	};
	
	for (const auto& e : tests) {
		unsigned char curr_s = ((e.md_input.status_nybble)|(e.md_input.ch));
		int curr_dt_size = midi_vl_field_size(e.dt_input);
		int curr_size = curr_dt_size+e.data_size;
		mtrk_event_t ev(e.dt_input,e.md_input);
		mtrk_event_unit_test_helper_t h(ev);

		EXPECT_EQ(ev.type(),classify_status_byte(curr_s));
		EXPECT_EQ(ev.delta_time(),e.dt_input);
		EXPECT_TRUE(h.is_small());
		EXPECT_FALSE(h.is_big());
		EXPECT_EQ(ev.status_byte(),curr_s);

		auto it_beg = ev.begin();
		auto it_evbeg = ev.event_begin();
		auto it_end = ev.end();
		EXPECT_TRUE(it_evbeg-it_beg,curr_dt_size);
		EXPECT_TRUE(it_end-it_beg<=ev.capacity());
		EXPECT_EQ(*it_evbeg,curr_s);
	}
}

