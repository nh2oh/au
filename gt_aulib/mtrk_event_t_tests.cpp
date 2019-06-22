#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include "..\aulib\input\midi\mtrk_event_t.h"
#include <vector>
#include <cstdint>


// 
// Test of the default-constructed value, a 0-length meta text event
// w/ a 0 delta-time:  0x00,0xFF,0x01,0x00,...
//
TEST(mtrk_event_t_tests, defaultConstructedValue) {
	// brace init form used to avoid vexing parse; could also do:
	// auto default_event = mtrk_event()
	mtrk_event_t default_event {mtrk_event_t()};
	
	EXPECT_EQ(default_event.delta_time(),0);
	EXPECT_EQ(default_event.type(),smf_event_type::meta);
	EXPECT_TRUE(default_event.is_small());
	EXPECT_FALSE(default_event.is_big());
	EXPECT_EQ(default_event.size(),4);
	EXPECT_EQ(default_event.data_size(),3);
}


//
// Tests of the mtrk_event_t(unsigned char *, int, unsigned char)
// ctor with a set of "small" (in the SBO-sense) midi_channel,meta,sysex_f0/f7
// events from the midi std and from files observed in the wild.  
//
// All events are constructed passing 0 for the running-status
//
TEST(mtrk_event_t_tests, assortedSMFEventsSmallZeroRS) {
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

		EXPECT_TRUE(c2.is_small());
		EXPECT_FALSE(c2.is_big());
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
TEST(mtrk_event_t_tests, randomSMFEvntsSmallRandomRS) {
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

	for (const auto& e : tests) {
		mtrk_event_t c2(e.data.data(),e.data.size(),e.midisb_prev_event);
		//EXPECT_EQ(c.type(),e.ans.type);  // TODO:  Add test when vs fixes its enum class issues
		EXPECT_EQ(c2.delta_time(),e.dt_value);

		EXPECT_TRUE(c2.is_small());
		EXPECT_FALSE(c2.is_big());
		EXPECT_EQ(c2.size(),e.data.size());
		EXPECT_EQ(c2.data_size(),e.data_length);
		EXPECT_TRUE(c2.validate());
		for (int i=0; i<e.data.size(); ++i) {
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
TEST(mtrk_event_t_tests, metaEventsSmallZeroRS) {
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
		//EXPECT_EQ(c.type(),e.ans.type);
		EXPECT_EQ(c2.type(),smf_event_type::meta);
		EXPECT_EQ(c2.delta_time(),e.ans.delta_time);
		
		EXPECT_TRUE(c2.is_small());
		EXPECT_FALSE(c2.is_big());
		EXPECT_EQ(c2.size(),e.ans.size);
		EXPECT_EQ(c2.data_size(),e.ans.data_size);
		EXPECT_TRUE(c2.validate());
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
TEST(mtrk_event_t_tests, MidiChEventStructCtorValidInputData) {
	struct test_t {
		uint32_t dt_input {0};
		midi_ch_event_t md_input {};
		//uint32_t dt_result {0};
		//midi_ch_event_t md_result {};
		uint32_t data_size {0};
	};
	// midi_ch_event_t {status, ch, p1, p2}
	std::vector<test_t> tests {
		{0, {note_on,0,57,32}, 3},
		{23, {note_off,1,57,32}, 3},
		{12354, {key_pressure,0,57,32}, 3},
		{0, {ctrl_change,15,72,100}, 3},
		{45541, {pitch_bend,0,127,127}, 3},
		// For 1-data-byte events, methods returning a midi_ch_event_t
		// should write 0x80u (128) for p2; this value is invalid for a 
		// data byte.  
		{785, {prog_change,14,127,0x80u}, 2},
		{2, {ch_pressure,2,0,0x80u}, 2}
	};
	
	for (const auto& e : tests) {
		unsigned char curr_s = (e.md_input.status_nybble + e.md_input.ch);
		mtrk_event_t ev(e.dt_input,e.md_input);
		EXPECT_EQ(ev.type(),smf_event_type::channel);

		EXPECT_EQ(ev.delta_time(),e.dt_input);
		EXPECT_TRUE(ev.is_small());
		EXPECT_FALSE(ev.is_big());
		EXPECT_EQ(ev.data_size(),e.data_size);
		EXPECT_TRUE(ev.validate());
		EXPECT_EQ(ev.status_byte(),curr_s);
		EXPECT_EQ(ev.running_status(),curr_s);

		auto curr_data = get_channel_event(ev);
		EXPECT_EQ(curr_data.status_nybble,e.md_input.status_nybble);
		EXPECT_EQ(curr_data.ch,e.md_input.ch);
		EXPECT_EQ(curr_data.p1,e.md_input.p1);
		EXPECT_EQ(curr_data.p2,e.md_input.p2);
	}
}



//
// Tests of the mtrk_event_t(uint32_t, const midi_ch_event_t&) ctor
// with _invalid_ data in the midi_ch_event_t struct.  
//
TEST(mtrk_event_t_tests, MidiChEventStructCtorInvalidInputData) {
	struct test_t {
		uint32_t dt_input {0};
		midi_ch_event_t md_input {};
		//uint32_t dt_result {0};
		midi_ch_event_t md_result {};
		uint32_t data_size {0};
	};
	// midi_ch_event_t {status, ch, p1, p2}
	std::vector<test_t> tests {
		{0, {note_on,16,57,32}, {note_on,0,57,32}, 3},  // Invalid channel
		{1, {note_on,127,57,32}, {note_on,15,57,32}, 3},  // Invalid channel
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
		//unsigned char curr_s_nybb = (e.md_input.status_nybble)|0x80u;
		unsigned char curr_s = e.md_result.status_nybble + e.md_result.ch;// + (e.md_input.ch)|0x0Fu;
		mtrk_event_t ev(e.dt_input,e.md_input);
		EXPECT_EQ(ev.type(),smf_event_type::channel);

		EXPECT_EQ(ev.delta_time(),e.dt_input);
		EXPECT_TRUE(ev.is_small());
		EXPECT_FALSE(ev.is_big());
		EXPECT_EQ(ev.data_size(),e.data_size);
		EXPECT_TRUE(ev.validate());
		EXPECT_EQ(ev.status_byte(),curr_s);
		EXPECT_EQ(ev.running_status(),curr_s);

		auto curr_data = get_channel_event(ev);
		EXPECT_EQ(curr_data.status_nybble,e.md_result.status_nybble);
		EXPECT_EQ(curr_data.ch,e.md_result.ch);
		EXPECT_EQ(curr_data.p1,e.md_result.p1);
		EXPECT_EQ(curr_data.p2,e.md_result.p2);
	}
}


//
// Tests of the copy ctor and copy assignment operator.  
//
// Events are "small" (in the SBO-sense) meta events from the midi std and
// from files observed in the wild.  All are initially constructed passing 
// 0 for the value of the running-status.  
//
TEST(mtrk_event_t_tests, metaEventsSmallCopyCtorAndCopyAssign) {
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
		mtrk_event_t c1(e.data(),e.size(),0);
		
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
		EXPECT_TRUE(c2.validate());
		for (int i=0; i<e.size(); ++i) {
			EXPECT_EQ(c2[i],e[i]);
			EXPECT_EQ(*(c2.data()+i),e[i]);
		}

		//---------------------------------------------------------------------------
		// copy assign:
		mtrk_event_t c3(tests[0].data(),tests[0].size(),0);
		c3 = c1;

		//EXPECT_EQ(c3.type(),c1.type());
		EXPECT_EQ(c3.type(),c1.type());
		EXPECT_EQ(c3.delta_time(),c1.delta_time());

		EXPECT_TRUE(c3.is_small());
		EXPECT_FALSE(c3.is_big());
		EXPECT_EQ(c3.size(),c1.size());
		EXPECT_EQ(c3.data_size(),c1.data_size());
		EXPECT_TRUE(c3.validate());
		for (int i=0; i<e.size(); ++i) {
			EXPECT_EQ(c3[i],e[i]);
			EXPECT_EQ(*(c3.data()+i),e[i]);
		}
	}
}

//
// Tests of the mtrk_event_t(unsigned char *, int, unsigned char) ctor
// 
// "Big" (in the SBO-sense) meta events from the midi std and from files
// observed in the wild.  All constructed passing 0 for teh value of the
// running-status.  
//
TEST(mtrk_event_t_tests, metaEventsBigZeroRS) {
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
			0x68,0x65,0x20,0x57,0x6F,0x72,0x6C,0x64,0x21},  // Sequence/track name (Hallelujah.mid)
		{false,0x00,33,32}},

		{{0x00,0xFF,0x01,0x7F,
			0x6D,0x6F,0x75,0x6E,0x74,0x20,0x6F,0x66,0x20,0x74,0x65,0x78,
			0x74,0x20,0x64,0x65,0x73,0x63,0x72,0x69,0x62,0x69,0x6E,0x67,
			0x20,0x61,0x6E,0x79,0x74,0x68,0x69,0x6E,0x67,0x2E,0x20,0x49,
			0x74,0x20,0x69,0x73,0x20,0x61,0x20,0x67,0x6F,0x6F,0x64,0x20,
			0x69,0x64,0x65,0x61,0x20,0x74,0x6F,0x20,0x70,0x75,0x74,0x20,
			0x61,0x20,0x74,0x65,0x78,0x74,0x20,0x65,0x76,0x65,0x6E,0x74,
			0x20,0x72,0x69,0x67,0x68,0x74,0x20,0x61,0x74,0x20,0x74,0x68,
			0x65,0x0D,0x0A,0x62,0x65,0x67,0x69,0x6E,0x6E,0x69,0x6E,0x67,
			0x20,0x6F,0x66,0x20,0x61,0x20,0x74,0x72,0x61,0x63,0x6B,0x2C,
			0x20,0x77,0x69,0x74,0x68,0x20,0x74,0x68,0x65,0x20,0x6E,0x61,
			0x6D,0x65,0x20,0x6F,0x66,0x20,0x74},  // Text event; 127 (0x7F) chars
		{false,0x00,131,130}},

		{{0x00,0xFF,0x01,0x5F,
			0x6D,0x6F,0x75,0x6E,0x74,0x20,0x6F,0x66,0x20,0x74,0x65,0x78,
			0x74,0x20,0x64,0x65,0x73,0x63,0x72,0x69,0x62,0x69,0x6E,0x67,
			0x20,0x61,0x6E,0x79,0x74,0x68,0x69,0x6E,0x67,0x2E,0x20,0x49,
			0x74,0x20,0x69,0x73,0x20,0x61,0x20,0x67,0x6F,0x6F,0x64,0x20,
			0x69,0x64,0x65,0x61,0x20,0x74,0x6F,0x20,0x70,0x75,0x74,0x20,
			0x61,0x20,0x74,0x65,0x78,0x74,0x20,0x65,0x76,0x65,0x6E,0x74,
			0x20,0x72,0x69,0x67,0x68,0x74,0x20,0x61,0x74,0x20,0x74,0x68,
			0x6D,0x65,0x20,0x6F,0x66,0x20,0x74,0x74,0x74,0x74,0x74},  // Text event; 95 (0x5F) chars
		{false,0x00,99,98}}
	};

	for (const auto& e : tests) {
		mtrk_event_t c2(e.bytes.data(),e.bytes.size(),0);
		//EXPECT_EQ(c.type(),e.ans.type);
		EXPECT_EQ(c2.type(),smf_event_type::meta);
		EXPECT_EQ(c2.delta_time(),e.ans.delta_time);

		EXPECT_FALSE(c2.is_small());
		EXPECT_TRUE(c2.is_big());
		EXPECT_EQ(c2.size(),e.ans.size);
		EXPECT_EQ(c2.data_size(),e.ans.data_size);
		EXPECT_TRUE(c2.validate());
		for (int i=0; i<e.ans.size; ++i) {
			EXPECT_EQ(c2[i],e.bytes[i]);
			EXPECT_EQ(*(c2.data()+i),e.bytes[i]);
		}
	}
}


//
// Tests of the copy ctor and copy assignment operator invoked between
// big events.  
//
TEST(mtrk_event_t_tests, metaEventsBigCopyCtorAndCopyAssign) {
	std::vector<std::vector<unsigned char>> tests {
		{0x00,0xFF,0x03,0x1D,  // 0x1D==29; Sequence/track name (Hallelujah.mid)
		0x48,0x61,0x6C,0x6C,0x65,0x6C,0x75,0x6A,
		0x61,0x68,0x21,0x20,0x4A,0x6F,0x79,0x20,0x74,0x6F,0x20,0x74,
		0x68,0x65,0x20,0x57,0x6F,0x72,0x6C,0x64,0x21},  

		{0x00,0xFF,0x01,0x7F,  // Text event; 127 (0x7F) chars
		0x6D,0x6F,0x75,0x6E,0x74,0x20,0x6F,0x66,0x20,0x74,0x65,0x78,
		0x74,0x20,0x64,0x65,0x73,0x63,0x72,0x69,0x62,0x69,0x6E,0x67,
		0x20,0x61,0x6E,0x79,0x74,0x68,0x69,0x6E,0x67,0x2E,0x20,0x49,
		0x74,0x20,0x69,0x73,0x20,0x61,0x20,0x67,0x6F,0x6F,0x64,0x20,
		0x69,0x64,0x65,0x61,0x20,0x74,0x6F,0x20,0x70,0x75,0x74,0x20,
		0x61,0x20,0x74,0x65,0x78,0x74,0x20,0x65,0x76,0x65,0x6E,0x74,
		0x20,0x72,0x69,0x67,0x68,0x74,0x20,0x61,0x74,0x20,0x74,0x68,
		0x65,0x0D,0x0A,0x62,0x65,0x67,0x69,0x6E,0x6E,0x69,0x6E,0x67,
		0x20,0x6F,0x66,0x20,0x61,0x20,0x74,0x72,0x61,0x63,0x6B,0x2C,
		0x20,0x77,0x69,0x74,0x68,0x20,0x74,0x68,0x65,0x20,0x6E,0x61,
		0x6D,0x65,0x20,0x6F,0x66,0x20,0x74},  

		{0x00,0xFF,0x01,0x5F,  // Text event; 95 (0x5F) chars
		0x6D,0x6F,0x75,0x6E,0x74,0x20,0x6F,0x66,0x20,0x74,0x65,0x78,
		0x74,0x20,0x64,0x65,0x73,0x63,0x72,0x69,0x62,0x69,0x6E,0x67,
		0x20,0x61,0x6E,0x79,0x74,0x68,0x69,0x6E,0x67,0x2E,0x20,0x49,
		0x74,0x20,0x69,0x73,0x20,0x61,0x20,0x67,0x6F,0x6F,0x64,0x20,
		0x69,0x64,0x65,0x61,0x20,0x74,0x6F,0x20,0x70,0x75,0x74,0x20,
		0x61,0x20,0x74,0x65,0x78,0x74,0x20,0x65,0x76,0x65,0x6E,0x74,
		0x20,0x72,0x69,0x67,0x68,0x74,0x20,0x61,0x74,0x20,0x74,0x68,
		0x6D,0x65,0x20,0x6F,0x66,0x20,0x74,0x74,0x74,0x74,0x74}  
	};
	
	bool first_iter=true;
	for (const auto& e : tests) {
		mtrk_event_t c1(e.data(),e.size(),0);

		//---------------------------------------------------------------------------
		// copy ctor:
		auto c2 = c1;

		EXPECT_EQ(c2.type(),c1.type());
		EXPECT_EQ(c2.delta_time(),c1.delta_time());

		EXPECT_FALSE(c2.is_small());
		EXPECT_TRUE(c2.is_big());
		EXPECT_EQ(c2.size(),c1.size());
		EXPECT_EQ(c2.data_size(),c1.data_size());
		EXPECT_TRUE(c2.validate());
		for (int i=0; i<e.size(); ++i) {
			EXPECT_EQ(c2[i],e[i]);
			EXPECT_EQ(*(c2.data()+i),e[i]);
		}

		//---------------------------------------------------------------------------
		// copy assign:
		// c1 is created from tests[i]; create a c3 different from c1
		// then overwrite it with c1.  
		int j=1;
		if (!first_iter) {
			j=0;
		}
		mtrk_event_t c3(tests[j].data(),tests[j].size(),0);
		c3 = c1;

		EXPECT_EQ(c3.type(),c1.type());
		EXPECT_EQ(c3.delta_time(),c1.delta_time());

		EXPECT_FALSE(c3.is_small());
		EXPECT_TRUE(c3.is_big());
		EXPECT_EQ(c3.size(),c1.size());
		EXPECT_EQ(c3.data_size(),c1.data_size());
		EXPECT_TRUE(c3.validate());
		for (int i=0; i<e.size(); ++i) {
			EXPECT_EQ(c3[i],e[i]);
			EXPECT_EQ(*(c3.data()+i),e[i]);
		}

		first_iter=false;
	}
}


// 
// Test of the move asssign operator by move-assigning a big event into
// a small event.  Meta events are random events generated by dbk 05/08/19.  
//
TEST(mtrk_event_t_tests, metaEventsMoveAssignBigIntoSmall) {
	struct test_t {
		std::vector<unsigned char> bytes {};
		uint32_t dtval {0};
		uint8_t type_byte;
		uint8_t payload_len;
		uint32_t data_size {0};
	};
	test_t small_data {
		{0x89,0x58,0xFF,0x58,0x04,0xDB,0x81,0x7A,0x0B},
		1240,0x58u,4,7
	};
	test_t big_data {
		{0x8B,0xD3,0x8E,0x62,0xFF,0x05,0x81,0x2C,0x4C,0x6F,0x72,0x65,0x6D,0x20,0x69,
			0x70,0x73,0x75,0x6D,0x20,0x73,0x6F,0x64,0x61,0x6C,0x65,0x73,0x20,0x6E,
			0x75,0x6E,0x63,0x20,0x76,0x75,0x6C,0x70,0x75,0x74,0x61,0x74,0x65,0x20,
			0x61,0x63,0x63,0x75,0x6D,0x73,0x61,0x6E,0x20,0x20,0x20,0x20,0x20,0x20,
			0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
			0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x6D,0x61,0x65,0x63,
			0x65,0x6E,0x61,0x73,0x20,0x76,0x69,0x74,0x61,0x65,0x2C,0x20,0x65,0x6C,
			0x69,0x74,0x20,0x65,0x6C,0x65,0x69,0x66,0x65,0x6E,0x64,0x20,0x63,0x6F,
			0x6E,0x76,0x61,0x6C,0x6C,0x69,0x73,0x20,0x6E,0x65,0x71,0x75,0x65,0x20,
			0x66,0x61,0x6D,0x65,0x73,0x20,0x64,0x69,0x61,0x6D,0x20,0x63,0x6F,0x6E,
			0x73,0x65,0x71,0x75,0x61,0x74,0x2C,0x70,0x6F,0x72,0x74,0x61,0x20,0x63,
			0x75,0x72,0x61,0x65,0x20,0x67,0x72,0x61,0x76,0x69,0x64,0x61,0x20,0x6C,
			0x65,0x6F,0x20,0x66,0x61,0x75,0x63,0x69,0x62,0x75,0x73},
		24430434,0x05u,172,176
	};

	mtrk_event_t small(small_data.bytes.data(),small_data.bytes.size(),0x00);
	EXPECT_EQ(small.delta_time(),small_data.dtval);
	EXPECT_EQ(small.type(),smf_event_type::meta);
	EXPECT_TRUE(small.is_small());
	EXPECT_FALSE(small.is_big());
	EXPECT_EQ(small.size(),small_data.bytes.size());
	EXPECT_EQ(small.data_size(),small_data.data_size);

	mtrk_event_t big(big_data.bytes.data(),big_data.bytes.size(),0x00);
	EXPECT_EQ(big.delta_time(),big_data.dtval);
	EXPECT_EQ(big.type(),smf_event_type::meta);
	EXPECT_TRUE(big.is_big());
	EXPECT_FALSE(big.is_small());
	EXPECT_EQ(big.size(),big_data.bytes.size());
	EXPECT_EQ(big.data_size(),big_data.data_size);
	
	//-------------------------------------------------------------------------
	// Force use of move assign w/ std::move()
	auto small_mvinto = small;
	auto big_mvfrom = big;
	small_mvinto=std::move(big_mvfrom);
	// big now has the moved-from state of a 0-length text event
	EXPECT_EQ(big_mvfrom.delta_time(),0);
	EXPECT_EQ(big_mvfrom.type(),smf_event_type::meta);
	EXPECT_FALSE(big_mvfrom.is_big());
	EXPECT_TRUE(big_mvfrom.is_small());
	EXPECT_EQ(big_mvfrom.size(),4);
	EXPECT_EQ(big_mvfrom.data_size(),3);
	// small has the values as if constructed from big_data
	EXPECT_EQ(small_mvinto.delta_time(),big_data.dtval);
	EXPECT_EQ(small_mvinto.type(),smf_event_type::meta);
	EXPECT_TRUE(small_mvinto.is_big());
	EXPECT_FALSE(small_mvinto.is_small());
	EXPECT_EQ(small_mvinto.size(),big_data.bytes.size());
	EXPECT_EQ(small_mvinto.data_size(),big_data.data_size);

	//-------------------------------------------------------------------------
	// Invoke move-assign by assigning from a temporary
	small_mvinto = small;
	small_mvinto = mtrk_event_t(big_data.bytes.data(),big_data.bytes.size(),0x00);
	// small has the values as if constructed from big_data
	EXPECT_EQ(small_mvinto.delta_time(),big_data.dtval);
	EXPECT_EQ(small_mvinto.type(),smf_event_type::meta);
	EXPECT_TRUE(small_mvinto.is_big());
	EXPECT_FALSE(small_mvinto.is_small());
	EXPECT_EQ(small_mvinto.size(),big_data.bytes.size());
	EXPECT_EQ(small_mvinto.data_size(),big_data.data_size);
}


// 
// Test of the move asssign operator by move-assigning a small event into
// a big event.  Meta events are random events generated by dbk 05/08/19.  
//
TEST(mtrk_event_t_tests, metaEventsMoveAssignSmallIntoBig) {
	struct test_t {
		std::vector<unsigned char> bytes {};
		uint32_t dtval {0};
		uint8_t type_byte;
		uint8_t payload_len;
		uint32_t data_size {0};
	};
	test_t small_data {
		{0x89,0x58,0xFF,0x58,0x04,0xDB,0x81,0x7A,0x0B},
		1240,0x58u,4,7
	};
	test_t big_data {
		{0x8B,0xD3,0x8E,0x62,0xFF,0x05,0x81,0x2C,0x4C,0x6F,0x72,0x65,0x6D,0x20,0x69,
			0x70,0x73,0x75,0x6D,0x20,0x73,0x6F,0x64,0x61,0x6C,0x65,0x73,0x20,0x6E,
			0x75,0x6E,0x63,0x20,0x76,0x75,0x6C,0x70,0x75,0x74,0x61,0x74,0x65,0x20,
			0x61,0x63,0x63,0x75,0x6D,0x73,0x61,0x6E,0x20,0x20,0x20,0x20,0x20,0x20,
			0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
			0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x6D,0x61,0x65,0x63,
			0x65,0x6E,0x61,0x73,0x20,0x76,0x69,0x74,0x61,0x65,0x2C,0x20,0x65,0x6C,
			0x69,0x74,0x20,0x65,0x6C,0x65,0x69,0x66,0x65,0x6E,0x64,0x20,0x63,0x6F,
			0x6E,0x76,0x61,0x6C,0x6C,0x69,0x73,0x20,0x6E,0x65,0x71,0x75,0x65,0x20,
			0x66,0x61,0x6D,0x65,0x73,0x20,0x64,0x69,0x61,0x6D,0x20,0x63,0x6F,0x6E,
			0x73,0x65,0x71,0x75,0x61,0x74,0x2C,0x70,0x6F,0x72,0x74,0x61,0x20,0x63,
			0x75,0x72,0x61,0x65,0x20,0x67,0x72,0x61,0x76,0x69,0x64,0x61,0x20,0x6C,
			0x65,0x6F,0x20,0x66,0x61,0x75,0x63,0x69,0x62,0x75,0x73},
		24430434,0x05u,172,176
	};

	mtrk_event_t small(small_data.bytes.data(),small_data.bytes.size(),0x00);
	EXPECT_EQ(small.delta_time(),small_data.dtval);
	EXPECT_EQ(small.type(),smf_event_type::meta);
	EXPECT_TRUE(small.is_small());
	EXPECT_FALSE(small.is_big());
	EXPECT_EQ(small.size(),small_data.bytes.size());
	EXPECT_EQ(small.data_size(),small_data.data_size);

	mtrk_event_t big(big_data.bytes.data(),big_data.bytes.size(),0x00);
	EXPECT_EQ(big.delta_time(),big_data.dtval);
	EXPECT_EQ(big.type(),smf_event_type::meta);
	EXPECT_TRUE(big.is_big());
	EXPECT_FALSE(big.is_small());
	EXPECT_EQ(big.size(),big_data.bytes.size());
	EXPECT_EQ(big.data_size(),big_data.data_size);
	
	//-------------------------------------------------------------------------
	// Force use of move assign w/ std::move()
	auto big_mvinto = big;
	auto small_mvfrom = small;
	big_mvinto=std::move(small_mvfrom);
	// small_mvfrom now has the moved-from state of a 0-length text event
	EXPECT_EQ(small_mvfrom.delta_time(),0);
	EXPECT_EQ(small_mvfrom.type(),smf_event_type::meta);
	EXPECT_FALSE(small_mvfrom.is_big());
	EXPECT_TRUE(small_mvfrom.is_small());
	EXPECT_EQ(small_mvfrom.size(),4);
	EXPECT_EQ(small_mvfrom.data_size(),3);
	// big_mvinto (which is no longer "big") has the values as if 
	// constructed from small_data
	EXPECT_EQ(big_mvinto.delta_time(),small_data.dtval);
	EXPECT_EQ(big_mvinto.type(),smf_event_type::meta);
	EXPECT_FALSE(big_mvinto.is_big());
	EXPECT_TRUE(big_mvinto.is_small());
	EXPECT_EQ(big_mvinto.size(),small_data.bytes.size());
	EXPECT_EQ(big_mvinto.data_size(),small_data.data_size);

	//-------------------------------------------------------------------------
	// Invoke move-assign by assigning from a temporary
	big_mvinto = big;
	big_mvinto = mtrk_event_t(small_data.bytes.data(),small_data.bytes.size(),0x00);
	// big_mvinto (which is no longer "big") has the values as if 
	// constructed from small_data
	EXPECT_EQ(big_mvinto.delta_time(),small_data.dtval);
	EXPECT_EQ(big_mvinto.type(),smf_event_type::meta);
	EXPECT_FALSE(big_mvinto.is_big());
	EXPECT_TRUE(big_mvinto.is_small());
	EXPECT_EQ(big_mvinto.size(),small_data.bytes.size());
	EXPECT_EQ(big_mvinto.data_size(),small_data.data_size);
}


// 
// Which meta events have a text field?
//
TEST(mtrk_event_t_tests, metaEventTypeHasText) {
	struct test_t {
		meta_event_t t {meta_event_t::unknown};
		bool ans_hastext {false};
	};
	std::vector<test_t> tests {
		{meta_event_t::seqn,false},
		{meta_event_t::text,true},
		{meta_event_t::copyright,true},
		{meta_event_t::trackname,true},
		{meta_event_t::instname,true},
		{meta_event_t::lyric,true},
		{meta_event_t::marker,true},
		{meta_event_t::cuepoint,true},
		{meta_event_t::chprefix,false},
		{meta_event_t::eot,false},
		{meta_event_t::tempo,false},
		{meta_event_t::smpteoffset,false},
		{meta_event_t::timesig,false},
		{meta_event_t::keysig,false},
		{meta_event_t::seqspecific,false},
		{meta_event_t::invalid,false},
		{meta_event_t::unknown,false}
	};

	for (const auto& e : tests) {
		EXPECT_EQ(meta_hastext_impl(static_cast<uint16_t>(e.t)),e.ans_hastext);
	}

}


